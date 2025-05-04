#include "controlclient.h"
#include "messagequeue.h"
#include "osdep.h"
#include "subspacegame.h"
#include "version.h"

#include "ftl/stdlib.h"
#include "atomic.h"
#include "minicrt.h"
#include "net.h"

typedef struct ControlClient {
    atomic(bool) shouldExit;

    socket_t notifysock;   // dummy socket to wake up the thread
    int notifyport;

    lock_t lock;
    MessageQueue* inbound;
    MessageQueue* outbound;
    MessageQueue* inbound_secondary;
    MessageQueue* outbound_secondary;
    hashtbl handlers;
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
        lock_acq(&client.lock);
        // swap queues while locked
        MessageQueue* oqueue      = client.outbound;
        client.outbound           = client.outbound_secondary;
        client.outbound_secondary = oqueue;
        lock_rel(&client.lock);

        for (int i = 0; i < oqueue->nmsgs; i++) {
            controlPutMsg(&control, &oqueue->msgs[i]->hdr, oqueue->msgs[i]->fields);
        }
        msgqClear(oqueue);

        if (isconn) {
            controlSend(&control);

            // read any inbound messages and queue them
            if (FD_ISSET(control.sock, &rset)) {
                while (controlMsgReady(&control)) {
                    ControlMsg* msg = controlGetMsg(&control, CF_ALLOC_AUTO);

                    lock_acq(&client.lock);
                    controlclientcb_t cb = (controlclientcb_t)_hashtbl_get(&client.handlers,
                                                                           (uintptr_t)msg->hdr.cmd);
                    if (cb) {
                        msgqAdd(client.inbound, msg, cb);
                    } else {
                        controlMsgFree(msg, CF_ALLOC_AUTO);
                    }
                    lock_rel(&client.lock);
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

    lock_init(&client.lock);
    hashtbl_init(&client.handlers, 16, HT_STRING_KEYS);
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
    lock_acq(&client.lock);
    msgqAdd(client.outbound, msg, NULL);
    lock_rel(&client.lock);
    controlClientNotify();
}

void controlClientProcess(void)
{
    lock_acq(&client.lock);
    // swap queues while locked
    MessageQueue* queue      = client.inbound;
    client.inbound           = client.inbound_secondary;
    client.inbound_secondary = queue;
    lock_rel(&client.lock);

    for (int i = 0; i < queue->nmsgs; i++) {
        if (queue->cbs[i])
            queue->cbs[i](queue->msgs[i]);
    }
    msgqClear(queue);
}

void controlClientRegister(const char* cmd, controlclientcb_t cb)
{
    lock_acq(&client.lock);
    hashtbl_add(&client.handlers, cmd, cb);
    lock_rel(&client.lock);
}
