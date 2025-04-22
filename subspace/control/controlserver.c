#include "controlserver.h"
#include <cx/thread.h>
#include <stdlib.h>
#include "controlclient.h"

#include <cx/container.h>
#include <cx/math.h>
#include <cx/thread.h>
#include <cx/thread/prqueue.h>
#include "net.h"

#include "control/cmds/gamestart.h"

saDeclare(socket_t);
saDeclarePtr(ControlState);

typedef struct ControlServer {
    Subspace* subspace;

    Thread* thread;
    sa_ControlClient clients;
    atomic(ptr) active;   // currently active client to send messages to

    socket_t svrsock;
    int port;

    socket_t notifysock;   // dummy socket to wake up the thread
    int notifyport;

    RWLock handler_lock;
    hashtable handlers;
} ControlServer;

ControlServer svr;

static int controlThread(Thread* self)
{
    fd_set rset;
    fd_set wset;
    struct timeval sto = { 0 };
    sto.tv_sec         = 10;
    sa_int32 removedConns;

    saInit(&removedConns, int32, 4, SA_Sorted);

    while (thrLoop(self)) {
        int maxfd = 0;

        FD_ZERO(&rset);
        FD_SET(svr.svrsock, &rset);
        FD_SET(svr.notifysock, &rset);
        maxfd = max(maxfd, svr.svrsock + 1);
        maxfd = max(maxfd, svr.notifysock + 1);

        FD_ZERO(&wset);
        foreach (sarray, idx, ControlClient*, cc, svr.clients) {
            socket_t sock = cclientSock(cc);
            FD_SET(sock, &rset);
            if (cclientSendPending(cc)) {
                FD_SET(sock, &wset);
            }
            maxfd = max(maxfd, sock + 1);
        }

        select(maxfd, &rset, &wset, NULL, &sto);

        if (FD_ISSET(svr.svrsock, &rset)) {
            struct sockaddr_in addr = { 0 };
            int addrlen             = sizeof(addr);
            // new connection inbound
            socket_t newsock        = accept(svr.svrsock, (struct sockaddr*)&addr, &addrlen);

            if (newsock) {
                ControlClient* ncli = cclientCreate(svr.subspace, newsock);
                saPushC(&svr.clients, object, &ncli);
            }
        }

        // clear out the single byte sent to the notify socket
        if (FD_ISSET(svr.notifysock, &rset)) {
            char tmp;
            recv(svr.notifysock, &tmp, 1, 0);
        }

        foreach (sarray, idx, ControlClient*, cc, svr.clients) {
            // send any outbound data
            cclientSend(cc);

            if (FD_ISSET(cclientSock(cc), &rset))
                cclientRecv(cc);

            if (cclientClosed(cc)) {
                // connection was closed, remove this one from the list
                saPush(&removedConns, int32, idx);
            }
        }

        for (int i = saSize(removedConns) - 1; i >= 0; --i) {
            saRemove(&svr.clients, removedConns.a[i]);
        }
        saClear(&removedConns);
    }

    netClose(svr.svrsock);
    netClose(svr.notifysock);
    svr.svrsock    = 0;
    svr.notifysock = 0;
    return 0;
}

bool controlServerStart(Subspace* ss)
{
    memset(&svr, 0, sizeof(ControlServer));
    svr.subspace = ss;
    rwlockInit(&svr.handler_lock);
    htInit(&svr.handlers, string, ptr, 16);
    saInit(&svr.clients, object, 4);
    srand(time(NULL));

    PcgState pcg;
    pcgAutoSeed(&pcg);

    // try to bind to a random port
    int i;
    for (i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        svr.svrsock               = socket(PF_INET, SOCK_STREAM, 0);
        svr.port                  = ss->port > 0 ? ss->port : pcgRange(&pcg, 1025, 65535);
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(ss->listenaddr);
        addr.sin_port             = htons(svr.port);

        if (bind(svr.svrsock, (struct sockaddr*)&addr, sizeof(addr)) == 0 &&
            listen(svr.svrsock, 5) == 0)
            break;

        netClose(svr.svrsock);
        svr.svrsock = 0;
    }
    netSetNonblock(svr.svrsock, true);

    if (svr.svrsock == 0)
        return false;

    for (i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        svr.notifysock            = socket(PF_INET, SOCK_DGRAM, 0);
        svr.notifyport            = pcgRange(&pcg, 1025, 65535);
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
        addr.sin_port             = htons(svr.notifyport);

        if (bind(svr.notifysock, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            break;

        netClose(svr.notifysock);
        svr.notifysock = 0;
    }
    netSetNonblock(svr.notifysock, true);

    if (svr.notifysock == 0)
        return false;

    // register all the command handlers
    CmdGameStart_register();

    // start up server thread
    svr.thread = thrCreate(controlThread, _S"Control Server", stvNone);

    return (svr.thread != NULL);
}

void controlServerNotify(void)
{
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
    addr.sin_port             = htons(svr.notifyport);

    char nothing = '\0';
    sendto(svr.notifysock, &nothing, 1, 0, (struct sockaddr*)&addr, sizeof(addr));
}

bool controlServerRegisterHandler(const char* cmd, ctask_factory_t handler)
{
    withWriteLock (&svr.handler_lock) {
        htInsert(&svr.handlers, strref, (strref)cmd, ptr, handler, HT_Ignore);
    }
    return true;
}

ctask_factory_t controlServerGetHandler(const char* cmd)
{
    ctask_factory_t ret = NULL;
    withReadLock (&svr.handler_lock) {
        htFind(svr.handlers, strref, (strref)cmd, ptr, &ret);
    }
    return ret;
}

void controlServerStop(void)
{
    thrRequestExit(svr.thread);
    controlServerNotify();
    thrShutdown(svr.thread);
    thrRelease(&svr.thread);
    rwlockDestroy(&svr.handler_lock);
    htDestroy(&svr.handlers);
}
