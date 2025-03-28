// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "taskrequiresresource.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/taskqueue/taskqueue.h>
#include <cx/taskqueue/task/complextask.h>

_objfactory_guaranteed TaskRequiresResource* TaskRequiresResource_create(_In_ TaskResource* res)
{
    TaskRequiresResource* self;
    self = objInstCreate(TaskRequiresResource);

    self->res = objAcquire(res);

    objInstInit(self);
    return self;
}

uint32 TaskRequiresResource_state(_In_ TaskRequiresResource* self, ComplexTask* task)
{
    return self->owned ?
        TASK_Requires_Ok :
        (taskresourceCanAcquire(self->res, task) ? TASK_Requires_Acquire : TASK_Requires_Wait);
}

int64 TaskRequiresResource_progress(_In_ TaskRequiresResource* self)
{
    return -1;
}

bool TaskRequiresResource_tryAcquire(_In_ TaskRequiresResource* self, ComplexTask* task)
{
    if (self->owned)
        return false;

    if (taskresourceTryAcquire(self->res, task)) {
        self->owned = true;
        return true;
    }

    return false;
}

bool TaskRequiresResource_release(_In_ TaskRequiresResource* self, ComplexTask* task)
{
    if (!self->owned)
        return false;

    taskresourceRelease(self->res, task);
    self->owned = false;
    return true;
}

void TaskRequiresResource_cancel(_In_ TaskRequiresResource* self)
{
}

bool TaskRequiresResource_registerTask(_In_ TaskRequiresResource* self, _In_ ComplexTask* task)
{
    return taskresourceRegisterTask(self->res, task);
}

void TaskRequiresResource_destroy(_In_ TaskRequiresResource* self)
{
    // Autogen begins -----
    objRelease(&self->res);
    // Autogen ends -------
}

// Autogen begins -----
#include "taskrequiresresource.auto.inc"
// Autogen ends -------
