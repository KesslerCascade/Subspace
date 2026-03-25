#include "controlclient.h"
#include "messagequeue.h"
#include "osdep.h"
#include "subspacegame.h"
#include "version.h"

#include "ftl/stdlib.h"
#include "net.h"

typedef struct ControlClient {
    atomic(bool) shouldExit;

    socket_t notifysock;   // dummy socket to wake up the thread
    int notifyport;

    Mutex lock;
    MessageQueue* inbound;
    MessageQueue* outbound;
    MessageQueue* inbound_secondary;
    MessageQueue* outbound_secondary;
    bool outbound_pending;
    bool outbound_ready;
    hashtable handlers;
} ControlClient;

ControlState control;
ControlClient client;

static int controlThread(void* data)
{
    fd_set rset, wset;
    struct timeval sto = { 0 };
    sto.tv_sec         = 10;

    while (!atomicLoad(bool, &client.shouldExit, Relaxed)) {
        bool isconn = control.sock && !control.closed;
        FD_ZERO(&rset);
        FD_ZERO(&wset);
        FD_SET(client.notifysock, &rset);
        int maxfd = client.notifysock + 1;

        if (isconn) {
            FD_SET(control.sock, &rset);
            if (sbufCAvail(control.sendbuf) > 0)
                FD_SET(control.sock, &wset);
            maxfd = max(maxfd, control.sock + 1);
        }

        select(maxfd, &rset, &wset, NULL, &sto);

        // clear out the single byte sent to the notify socket
        if (FD_ISSET(client.notifysock, &rset)) {
            char tmp;
            recv(client.notifysock, &tmp, 1, 0);
        }

        // swap queues and process any outbound messages
        mutexAcquire(&client.lock);
        MessageQueue* oqueue = NULL;
        if (client.outbound_ready) {
            // swap queues while locked
            oqueue                    = client.outbound;
            client.outbound           = client.outbound_secondary;
            client.outbound_secondary = oqueue;
            client.outbound_ready     = false;
        }
        mutexRelease(&client.lock);

        if (oqueue) {
            int curprio  = 0;
            int nextprio = 0;
            do {
                nextprio = 0x7fffffff;

                for (int i = 0; i < oqueue->nmsgs; i++) {
                    ControlMsg* msg = oqueue->msgs[i];
                    if (msg->priority == curprio) {
                        controlSendMsg(&control, msg);
                    } else {
                        // get the smallest higher priority to be sent next
                        if (msg->priority > curprio && msg->priority < nextprio)
                            nextprio = msg->priority;
                    }
                }

                curprio = nextprio;
            } while (nextprio != 0x7fffffff);
            msgqClear(oqueue);
        }

        if (isconn) {
            controlSendBuffer(&control);

            // read any inbound messages and queue them
            if (FD_ISSET(control.sock, &rset)) {
                while (controlMsgReady(&control)) {
                    ControlMsg* msg = controlRecvMsg(&control);

                    withMutex (&client.lock) {
                        controlclientcb_t cb;
                        if (htFind(client.handlers, strref, (strref)msg->hdr.cmd, ptr, &cb)) {
                            msgqAdd(client.inbound, msg, cb);
                        } else {
                            controlMsgDestroy(msg);
                        }
                    }
                }
            }
        }
    }
    return 0;
}

bool controlClientStart(void)
{
    memset(&client, 0, sizeof(ControlClient));
    srand(time(NULL));

    for (int i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        client.notifysock         = socket(PF_INET, SOCK_DGRAM, 0);
        client.notifyport         = rand() % 64512 + 1024;
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
        addr.sin_port             = htons(client.notifyport);

        if (bind(client.notifysock, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            break;

        netClose(client.notifysock);
        client.notifysock = 0;
    }
    netSetNonblock(client.notifysock, true);

    if (client.notifysock == 0)
        return false;

    mutexInit(&client.lock);
    htInit(&client.handlers, string, ptr, 16);
    client.inbound            = msgqCreate(16, true);
    client.inbound_secondary  = msgqCreate(16, true);
    client.outbound           = msgqCreate(16, false);
    client.outbound_secondary = msgqCreate(16, false);
    osStartThread(controlThread, NULL);
    return true;
}

void controlClientStop(void)
{
    atomicStore(bool, &client.shouldExit, true, Relaxed);
    controlClientNotify();
}

void controlClientNotify(void)
{
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
    addr.sin_port             = htons(client.notifyport);

    char nothing = '\0';
    sendto(client.notifysock, &nothing, 1, 0, (struct sockaddr*)&addr, sizeof(addr));
}

void controlClientQueue(ControlMsg* msg)
{
    withMutex (&client.lock) {
        msgqAdd(client.outbound, msg, NULL);
        client.outbound_pending = true;
    }
}

void controlClientProcessInbound(void)
{
    mutexAcquire(&client.lock);
    // swap queues while locked
    MessageQueue* queue      = client.inbound;
    client.inbound           = client.inbound_secondary;
    client.inbound_secondary = queue;
    mutexRelease(&client.lock);

    for (int i = 0; i < queue->nmsgs; i++) {
        if (queue->cbs[i])
            queue->cbs[i](queue->msgs[i], queue->msgs[i]->fields);
    }
    msgqClear(queue);
}

void controlClientProcessOutbound(void)
{
    bool dosend = false;
    withMutex (&client.lock) {
        if (client.outbound_pending) {
            client.outbound_ready   = true;   // clear to send
            client.outbound_pending = false;
            dosend                  = true;
        }
    }

    // notify client thread to send
    if (dosend)
        controlClientNotify();
}

void controlClientRegister(strref cmd, controlclientcb_t cb)
{
    withMutex (&client.lock) {
        htInsert(&client.handlers, strref, cmd, ptr, cb);
    }
}

bool controlClientConnected(void)
{
    return (control.sock != 0 && !control.closed);
}
