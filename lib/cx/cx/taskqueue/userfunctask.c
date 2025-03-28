// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "userfunctask.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed UserFuncTask* UserFuncTask_create(UserTaskCB func, void* udata)
{
    UserFuncTask* self;
    self = objInstCreate(UserFuncTask);

    self->func  = func;
    self->udata = udata;

    objInstInit(self);

    return self;
}

uint32 UserFuncTask_run(_In_ UserFuncTask* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon)
{
    return (self->func && self->func(tq, self->udata)) ? TASK_Result_Success : TASK_Result_Failure;
}

// Autogen begins -----
#include "userfunctask.auto.inc"
// Autogen ends -------
