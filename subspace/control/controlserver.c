// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "controlserver.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "netsocket.h"

#include <cx/math.h>
#include "cmds.h"

_objfactory_guaranteed ControlServer* ControlServer_create(Subspace* subspace)
{
    ControlServer* self;
    self = objInstCreate(ControlServer);

    self->ss = subspace;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool ControlServer_init(_In_ ControlServer* self)
{
    self->preGameReady = trfifoCreate();

    // Autogen begins -----
    saInit(&self->clients, object, 1);
    rwlockInit(&self->handler_lock);
    htInit(&self->handlers, string, object, 16);
    return true;
    // Autogen ends -------
}

static int controlThread(Thread* self)
{
    ControlServer* svr = stvlNextObj(&self->args, ControlServer);
    fd_set rset;
    fd_set wset;
    struct timeval sto = { 0 };
    sto.tv_sec         = 10;
    sa_int32 removedConns;

    if (!svr)
        return 1;

    saInit(&removedConns, int32, 4, SA_Sorted);

    while (thrLoop(self)) {
        int maxfd = 0;

        FD_ZERO(&rset);
        FD_SET(svr->svrsock, &rset);
        FD_SET(svr->notifysock, &rset);
        maxfd = max(maxfd, svr->svrsock + 1);
        maxfd = max(maxfd, svr->notifysock + 1);

        FD_ZERO(&wset);
        foreach (sarray, idx, ControlClient*, cc, svr->clients) {
            socket_t sock = cclientSock(cc);
            FD_SET(sock, &rset);
            if (cclientSendPending(cc)) {
                FD_SET(sock, &wset);
            }
            maxfd = max(maxfd, sock + 1);
        }

        select(maxfd, &rset, &wset, NULL, &sto);

        if (FD_ISSET(svr->svrsock, &rset)) {
            struct sockaddr_in addr = { 0 };
            int addrlen             = sizeof(addr);
            // new connection inbound
            socket_t newsock        = accept(svr->svrsock, (struct sockaddr*)&addr, &addrlen);

            if (newsock) {
                logFmt(Verbose,
                       _S"Accepting connection from ${string}:${int}",
                       stvar(strref, (strref)inet_ntoa(addr.sin_addr)),
                       stvar(int32, ntohs(addr.sin_port)));
                ControlClient* ncli = cclientCreate(svr, newsock);
                saPushC(&svr->clients, object, &ncli);
            }
        }

        // clear out the single byte sent to the notify socket
        if (FD_ISSET(svr->notifysock, &rset)) {
            char tmp;
            recv(svr->notifysock, &tmp, 1, 0);
        }

        foreach (sarray, idx, ControlClient*, cc, svr->clients) {
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
            saRemove(&svr->clients, removedConns.a[i]);
        }
        saClear(&removedConns);
    }

    netClose(svr->svrsock);
    netClose(svr->notifysock);
    svr->svrsock    = 0;
    svr->notifysock = 0;
    saClear(&svr->clients);
    objRelease(&svr);
    return 0;
}

bool ControlServer_start(_In_ ControlServer* self)
{
    Subspace* ss = self->ss;
    PcgState pcg;
    pcgAutoSeed(&pcg);

    // try to bind to a random port
    int i;
    for (i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        self->svrsock             = socket(PF_INET, SOCK_STREAM, 0);
        self->port                = ss->port > 0 ? ss->port : pcgRange(&pcg, 1025, 65535);
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(ss->listenaddr);
        addr.sin_port             = htons(self->port);

        if (bind(self->svrsock, (struct sockaddr*)&addr, sizeof(addr)) == 0 &&
            listen(self->svrsock, 5) == 0) {
            logFmt(Verbose,
                   _S"Listening on ${string}:${int}",
                   stvar(strref, (strref)inet_ntoa(addr.sin_addr)),
                   stvar(int32, self->port));
            break;
        }

        netClose(self->svrsock);
        self->svrsock = 0;
    }

    if (self->svrsock == 0)
        return false;
    netSetNonblock(self->svrsock, true);

    for (i = 0; i < 10000; i++) {
        struct sockaddr_in addr = { 0 };

        self->notifysock          = socket(PF_INET, SOCK_DGRAM, 0);
        self->notifyport          = pcgRange(&pcg, 1025, 65535);
        addr.sin_family           = AF_INET;
        addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
        addr.sin_port             = htons(self->notifyport);

        if (bind(self->notifysock, (struct sockaddr*)&addr, sizeof(addr)) == 0)
            break;

        netClose(self->notifysock);
        self->notifysock = 0;
    }

    if (self->notifysock == 0)
        return false;
    netSetNonblock(self->notifysock, true);

    // register all the command handlers
    registerCmds(self);

    // start up server thread
    ControlServer* csthr = objAcquire(self);   // for the thread to own
    self->thread         = thrCreate(controlThread, _S"Control Server", stvar(object, csthr));

    return (self->thread != NULL);
}

void ControlServer_stop(_In_ ControlServer* self)
{
    thrRequestExit(self->thread);
    cserverNotify(self);
    thrShutdown(self->thread);
    thrRelease(&self->thread);
    htClear(&self->handlers);
}

void ControlServer_notify(_In_ ControlServer* self)
{
    struct sockaddr_in addr   = { 0 };
    addr.sin_family           = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(0x7f000001);
    addr.sin_port             = htons(self->notifyport);

    char nothing = '\0';
    sendto(self->notifysock, &nothing, 1, 0, (struct sockaddr*)&addr, sizeof(addr));
}

bool ControlServer_registerHandler(_In_ ControlServer* self, _In_opt_ strref cmd,
                                   controlservercb_t cb, bool needinst, TaskResource* require)
{
    ControlHandler* hobj = controlhandlerCreate(cb, needinst, require);
    withWriteLock (&self->handler_lock) {
        htInsertC(&self->handlers, strref, cmd, object, &hobj);
    }
    return true;
}

ControlHandler* ControlServer_getHandler(_In_ ControlServer* self, _In_opt_ strref cmd)
{
    ControlHandler* ret = NULL;
    withReadLock (&self->handler_lock) {
        htelem elem = htFind(self->handlers, strref, cmd, none, NULL);
        if (elem)
            ret = (ControlHandler*)hteVal(self->handlers, object, elem);
    }
    return ret;
}

int ControlServer_port(_In_ ControlServer* self)
{
    return self->port;
}

void ControlServer_destroy(_In_ ControlServer* self)
{
    // Autogen begins -----
    objRelease(&self->thread);
    saDestroy(&self->clients);
    rwlockDestroy(&self->handler_lock);
    htDestroy(&self->handlers);
    objRelease(&self->preGameReady);
    // Autogen ends -------
}

_objfactory_guaranteed ControlHandler* ControlHandler_create(controlservercb_t cb, bool needinst, TaskResource* require)
{
    ControlHandler* self;
    self = objInstCreate(ControlHandler);

    self->cb       = cb;
    self->needinst = needinst;
    if (require)
        self->require = objAcquire(require);

    objInstInit(self);
    return self;
}

void ControlHandler_destroy(_In_ ControlHandler* self)
{
    // Autogen begins -----
    objRelease(&self->require);
    // Autogen ends -------
}

// Autogen begins -----
#include "controlserver.auto.inc"
// Autogen ends -------
