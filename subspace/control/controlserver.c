#include "controlserver.h"
#include <cx/thread.h>
#include <stdlib.h>

ControlServer svr;

int controlThread(Thread* self)
{
    fd_set rset;
    fd_set wset;
    struct timeval sto = { 0 };
    sto.tv_sec = 10;
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
        foreach(sarray, idx, ControlState *, cs, svr.clients) {
            FD_SET(cs->sock, &rset);
            if (sbufCAvail(cs->sendbuf) > 0) {
                FD_SET(cs->sock, &wset);
            }
            maxfd = max(maxfd, cs->sock + 1);
        }

        select(maxfd, &rset, &wset, NULL, &sto);

        if (FD_ISSET(svr.svrsock, &rset)) {
            struct sockaddr_in addr = { 0 };
            int addrlen             = sizeof(addr);
            // new connection inbound
            socket_t newsock        = accept(svr.svrsock, (struct sockaddr*)&addr, &addrlen);

            if (newsock) {
                ControlState* nctl = xaAlloc(sizeof(ControlState), XA_Zero);
                controlInit(nctl, newsock);
                saPush(&svr.clients, ptr, nctl);
            }
        }

        // clear out the single byte sent to the notify socket
        if (FD_ISSET(svr.notifysock, &rset)) {
            char tmp;
            recv(svr.notifysock, &tmp, 1, 0);
        }

        foreach(sarray, idx, ControlState *, cs, svr.clients) {
            controlSend(cs);    // send any outbound data

            if (FD_ISSET(cs->sock, &rset)) {
                if (controlMsgReady(cs)) {
                    ControlMsg* temp = controlGetMsg(cs, CF_ALLOC_AUTO);
                    controlMsgFree(temp, CF_ALLOC_AUTO);
                };   // celebrate?
            }

            if (cs->closed) {
                // connection was closed, remove this one from the list
                saPush(&removedConns, int32, idx);
            }
        }

        for (int i = saSize(removedConns) - 1; i >= 0; --i) {
            int cidx = removedConns.a[i];
            netClose(svr.clients.a[cidx]->sock);
            controlStateDestroy(svr.clients.a[cidx]);
            xaFree(svr.clients.a[cidx]);
            saRemove(&svr.clients, cidx);
        }
        saClear(&removedConns);
    }

    netClose(svr.svrsock);
    netClose(svr.notifysock);
    svr.svrsock = 0;
    svr.notifysock = 0;
    return 0;
}

bool controlServerStart(void)
{
    memset(&svr, 0, sizeof(ControlServer));
    saInit(&svr.clients, ptr, 4);
    srand(time(NULL));

    // try to bind to a random port
    int i;
    for (i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        svr.svrsock               = socket(PF_INET, SOCK_STREAM, 0);
        svr.port                  = rand() % 64512 + 1024;
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
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
        svr.notifyport            = rand() % 64512 + 1024;
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

void controlServerStop(void)
{
    thrRequestExit(svr.thread);
    controlServerNotify();
    thrShutdown(svr.thread);
    thrRelease(&svr.thread);
}
