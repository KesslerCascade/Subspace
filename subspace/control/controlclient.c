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

_objfactory_guaranteed ControlClient* ControlClient_create(socket_t sock)
{
    ControlClient* self;
    self = objInstCreate(ControlClient);
    objInstInit(self);

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
    if (controlMsgReady(&self->state)) {
        ControlMsg* temp = controlGetMsg(&self->state, CF_ALLOC_AUTO);
        controlMsgFree(temp, CF_ALLOC_AUTO);
    };   // celebrate?
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

// Autogen begins -----
#include "controlclient.auto.inc"
// Autogen ends -------
