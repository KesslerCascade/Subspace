// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "controlclient.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gamemgr.h"
#include "controlserver.h"
#include "controltask.h"

_objfactory_guaranteed ControlClient* ControlClient_create(ControlServer* svr, socket_t sock)
{
    ControlClient* self;
    self = objInstCreate(ControlClient);
    objInstInit(self);

    self->svr = objGetWeak(ControlServer, svr);
    self->ss  = svr->ss;
    controlInit(&self->state, sock);

    return self;
}

_objinit_guaranteed bool ControlClient_init(_In_ ControlClient* self)
{
    prqInitDynamic(&self->outbound, 16, 32, 0, 0, 0);

    // Autogen begins -----
    return true;
    // Autogen ends -------
}

void ControlClient_destroy(_In_ ControlClient* self)
{
    ControlMsg* msg = NULL;
    while (msg = prqPop(&self->outbound)) {
        controlMsgFree(msg, CF_ALLOC_AUTO);
    }
    prqDestroy(&self->outbound);

    netClose(self->state.sock);
    controlStateDestroy(&self->state);
    // Autogen begins -----
    objDestroyWeak(&self->svr);
    objDestroyWeak(&self->inst);
    // Autogen ends -------
}

void ControlClient_send(_In_ ControlClient* self)
{
    ControlMsg* msg = NULL;
    while (msg = prqPop(&self->outbound)) {
        controlPutMsg(&self->state, &msg->hdr, msg->fields);
        controlMsgFree(msg, CF_ALLOC_AUTO);
    }
    controlSend(&self->state);
}

void ControlClient_recv(_In_ ControlClient* self)
{
    ControlServer* svr = objAcquireFromWeak(ControlServer, self->svr);
    if (!svr)
        return;

    while (controlMsgReady(&self->state)) {
        ControlMsg* msg = controlGetMsg(&self->state, CF_ALLOC_AUTO);
        if (!msg) {
            objRelease(&svr);
            return;
        }

        ControlHandler* handler = cserverGetHandler(svr, (strref)msg->hdr.cmd);
        if (handler) {
            ControlTask* task = controltaskCreate(self, handler, msg);
            tqRun(self->ss->workq, &task);
        } else {
            logFmt(Info,
                   _S"Unknown command received from client: ${string}",
                   stvar(strref, (strref)msg->hdr.cmd));
            controlMsgFree(msg, CF_ALLOC_AUTO);
        }
    };
    objRelease(&svr);
}

socket_t ControlClient_sock(_In_ ControlClient* self)
{
    return self->state.sock;
}

bool ControlClient_closed(_In_ ControlClient* self)
{
    return self->state.closed;
}

bool ControlClient_sendPending(_In_ ControlClient* self)
{
    return sbufCAvail(self->state.sendbuf) > 0;
}

void ControlClient_queue(_In_ ControlClient* self, ControlMsg* msg)
{
    ControlServer* svr = objAcquireFromWeak(ControlServer, self->svr);
    if (!svr)
        return;

    prqPush(&self->outbound, msg);
    cserverNotify(svr);

    objRelease(&svr);
}

// Autogen begins -----
#include "controlclient.auto.inc"
// Autogen ends -------
