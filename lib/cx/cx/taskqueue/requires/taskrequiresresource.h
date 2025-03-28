#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "taskrequires.h"
#include <cx/taskqueue/resource/taskresource.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskRequiresResource TaskRequiresResource;
typedef struct TaskRequiresResource_WeakRef TaskRequiresResource_WeakRef;
saDeclarePtr(TaskRequiresResource);
saDeclarePtr(TaskRequiresResource_WeakRef);

typedef struct TaskRequiresResource_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // Calculate the current state of the requirement.
    uint32 (*state)(_In_ void* self, ComplexTask* task);
    // If possible, return the last progress timestamp associated with the requirement, or -1 if not applicable.
    int64 (*progress)(_In_ void* self);
    // For requirements that involve resource acquisition, attempts to acquire the exclusive resource on behalf
    // of task. This should only be called if state() returns TASK_Requires_Acquire. If called, the caller MUST
    // then call release() when the task is finished running.
    // Implementions of this function must NOT block -- they should try to acquire a lock but return false
    // if it cannot be acquired.
    bool (*tryAcquire)(_In_ void* self, ComplexTask* task);
    // Releases the resource, MUST be paired with acquire and called with the same task used to acquire.
    bool (*release)(_In_ void* self, ComplexTask* task);
    // The task is being cancelled and wishes to cascade the cancellation to any dependencies
    void (*cancel)(_In_ void* self);
    // Requests that the requirement notify the task when conditions change that may change the state of the
    // requirement. The requirement must notify the task by advancing it out of the defer queue. This also registers
    // the task with shared resources if needed. Such as registration is a one-shot and is consumed when the
    // resource is acquired.
    bool (*registerTask)(_In_ void* self, _In_ ComplexTask* task);
} TaskRequiresResource_ClassIf;
extern TaskRequiresResource_ClassIf TaskRequiresResource_ClassIf_tmpl;

typedef struct TaskRequiresResource {
    union {
        TaskRequiresResource_ClassIf* _;
        void* _is_TaskRequiresResource;
        void* _is_TaskRequires;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 expires;        // Time after which this requires is considered timed out and expired, and will fail.
    bool owned;
    TaskResource* res;
} TaskRequiresResource;
extern ObjClassInfo TaskRequiresResource_clsinfo;
#define TaskRequiresResource(inst) ((TaskRequiresResource*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresResource)), (inst)))
#define TaskRequiresResourceNone ((TaskRequiresResource*)NULL)

typedef struct TaskRequiresResource_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskRequiresResource_WeakRef;
        void* _is_TaskRequires_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskRequiresResource_WeakRef;
#define TaskRequiresResource_WeakRef(inst) ((TaskRequiresResource_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresResource_WeakRef)), (inst)))

_objfactory_guaranteed TaskRequiresResource* TaskRequiresResource_create(_In_ TaskResource* res);
// TaskRequiresResource* taskrequiresresourceCreate(TaskResource* res);
#define taskrequiresresourceCreate(res) TaskRequiresResource_create(TaskResource(res))

// uint32 taskrequiresresourceState(TaskRequiresResource* self, ComplexTask* task);
//
// Calculate the current state of the requirement.
#define taskrequiresresourceState(self, task) (self)->_->state(TaskRequiresResource(self), ComplexTask(task))
// int64 taskrequiresresourceProgress(TaskRequiresResource* self);
//
// If possible, return the last progress timestamp associated with the requirement, or -1 if not applicable.
#define taskrequiresresourceProgress(self) (self)->_->progress(TaskRequiresResource(self))
// bool taskrequiresresourceTryAcquire(TaskRequiresResource* self, ComplexTask* task);
//
// For requirements that involve resource acquisition, attempts to acquire the exclusive resource on behalf
// of task. This should only be called if state() returns TASK_Requires_Acquire. If called, the caller MUST
// then call release() when the task is finished running.
// Implementions of this function must NOT block -- they should try to acquire a lock but return false
// if it cannot be acquired.
#define taskrequiresresourceTryAcquire(self, task) (self)->_->tryAcquire(TaskRequiresResource(self), ComplexTask(task))
// bool taskrequiresresourceRelease(TaskRequiresResource* self, ComplexTask* task);
//
// Releases the resource, MUST be paired with acquire and called with the same task used to acquire.
#define taskrequiresresourceRelease(self, task) (self)->_->release(TaskRequiresResource(self), ComplexTask(task))
// void taskrequiresresourceCancel(TaskRequiresResource* self);
//
// The task is being cancelled and wishes to cascade the cancellation to any dependencies
#define taskrequiresresourceCancel(self) (self)->_->cancel(TaskRequiresResource(self))
// bool taskrequiresresourceRegisterTask(TaskRequiresResource* self, ComplexTask* task);
//
// Requests that the requirement notify the task when conditions change that may change the state of the
// requirement. The requirement must notify the task by advancing it out of the defer queue. This also registers
// the task with shared resources if needed. Such as registration is a one-shot and is consumed when the
// resource is acquired.
#define taskrequiresresourceRegisterTask(self, task) (self)->_->registerTask(TaskRequiresResource(self), ComplexTask(task))

