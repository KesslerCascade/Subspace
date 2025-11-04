// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "control/controltask.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gameinst.h"

void ControlTask_destroy(_In_ ControlTask* self)
{
    controlMsgDestroy(self->msg);
    // Autogen begins -----
    objRelease(&self->client);
    objRelease(&self->handler);
    // Autogen ends -------
}

_objfactory_guaranteed ControlTask*
ControlTask_create(ControlClient* client, ControlHandler* handler, ControlMsg* msg)
{
    ControlTask* self;
    self = objInstCreate(ControlTask);

    self->client  = objAcquire(client);
    self->handler = objAcquire(handler);
    self->msg     = msg;

    objInstInit(self);

    if (handler->require)
        ctaskRequireResource(self, handler->require);

    return self;
}

uint32 ControlTask_run(_In_ ControlTask* self, _In_ TaskQueue* tq, _In_ TQWorker* worker,
                       _Inout_ TaskControl* tcon)
{
    GameInst* inst = objAcquireFromWeak(GameInst, self->client->inst);
    if (self->handler->needinst && !inst)
        return TASK_Result_Success;

    self->handler->cb(inst, self->client, self->msg, self->msg->fields);

    objRelease(&inst);

    return TASK_Result_Success;
}

// Autogen begins -----
// clang-format off
#include "control/controltask.auto.inc"
// clang-format on
// Autogen ends -------
