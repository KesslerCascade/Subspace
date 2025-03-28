// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "taskrequirestask.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>
#include <cx/taskqueue/task/complextask.h>

_objfactory_guaranteed TaskRequiresTask* TaskRequiresTask_create(_In_ Task* deptask, bool failok)
{
    TaskRequiresTask* self;
    self = objInstCreate(TaskRequiresTask);

    self->task = objAcquire(deptask);
    self->failok = failok;

    objInstInit(self);
    return self;
}

uint32 TaskRequiresTask_state(_In_ TaskRequiresTask* self, ComplexTask* task)
{
    uint32 state = taskState(self->task);

    if (state == TASK_Succeeded)
        return TASK_Requires_Ok_Permanent;
    else if (state == TASK_Failed)
        return self->failok ? TASK_Requires_Ok_Permanent : TASK_Requires_Fail_Permanent;
    else
        return TASK_Requires_Wait;
}

bool TaskRequiresTask_tryAcquire(_In_ TaskRequiresTask* self, ComplexTask* task)
{
    return false;
}

bool TaskRequiresTask_release(_In_ TaskRequiresTask* self, ComplexTask* task)
{
    return false;
}

void TaskRequiresTask_cancel(_In_ TaskRequiresTask* self)
{
    taskCancel(self->task);
}

bool TaskRequiresTask_registerTask(_In_ TaskRequiresTask* self, _In_ ComplexTask* task)
{
    Weak(ComplexTask)* wref = objGetWeak(ComplexTask, task);
    bool ret = cchainAttach(&self->task->oncomplete, ComplexTask_advanceCallback, stvar(weakref, wref));
    objDestroyWeak(&wref);

    return ret;
}

int64 TaskRequiresTask_progress(_In_ TaskRequiresTask* self)
{
    ComplexTask* ctask = objDynCast(ComplexTask, self->task);
    if (ctask)
        return ctask->lastprogress;
    return -1;
}

void TaskRequiresTask_destroy(_In_ TaskRequiresTask* self)
{
    // Autogen begins -----
    objRelease(&self->task);
    // Autogen ends -------
}

// Autogen begins -----
#include "taskrequirestask.auto.inc"
// Autogen ends -------
