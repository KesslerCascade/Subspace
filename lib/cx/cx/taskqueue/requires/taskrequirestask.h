#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "taskrequires.h"
#include <cx/taskqueue/task/task.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct TaskRequiresTask TaskRequiresTask;
typedef struct TaskRequiresTask_WeakRef TaskRequiresTask_WeakRef;
saDeclarePtr(TaskRequiresTask);
saDeclarePtr(TaskRequiresTask_WeakRef);

typedef struct TaskRequiresTask_ClassIf {
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
} TaskRequiresTask_ClassIf;
extern TaskRequiresTask_ClassIf TaskRequiresTask_ClassIf_tmpl;

typedef struct TaskRequiresTask {
    union {
        TaskRequiresTask_ClassIf* _;
        void* _is_TaskRequiresTask;
        void* _is_TaskRequires;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 expires;        // Time after which this requires is considered timed out and expired, and will fail.
    Task* task;
    bool failok;
} TaskRequiresTask;
extern ObjClassInfo TaskRequiresTask_clsinfo;
#define TaskRequiresTask(inst) ((TaskRequiresTask*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresTask)), (inst)))
#define TaskRequiresTaskNone ((TaskRequiresTask*)NULL)

typedef struct TaskRequiresTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskRequiresTask_WeakRef;
        void* _is_TaskRequires_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskRequiresTask_WeakRef;
#define TaskRequiresTask_WeakRef(inst) ((TaskRequiresTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresTask_WeakRef)), (inst)))

_objfactory_guaranteed TaskRequiresTask* TaskRequiresTask_create(_In_ Task* deptask, bool failok);
// TaskRequiresTask* taskrequirestaskCreate(Task* deptask, bool failok);
#define taskrequirestaskCreate(deptask, failok) TaskRequiresTask_create(Task(deptask), failok)

// uint32 taskrequirestaskState(TaskRequiresTask* self, ComplexTask* task);
//
// Calculate the current state of the requirement.
#define taskrequirestaskState(self, task) (self)->_->state(TaskRequiresTask(self), ComplexTask(task))
// int64 taskrequirestaskProgress(TaskRequiresTask* self);
//
// If possible, return the last progress timestamp associated with the requirement, or -1 if not applicable.
#define taskrequirestaskProgress(self) (self)->_->progress(TaskRequiresTask(self))
// bool taskrequirestaskTryAcquire(TaskRequiresTask* self, ComplexTask* task);
//
// For requirements that involve resource acquisition, attempts to acquire the exclusive resource on behalf
// of task. This should only be called if state() returns TASK_Requires_Acquire. If called, the caller MUST
// then call release() when the task is finished running.
// Implementions of this function must NOT block -- they should try to acquire a lock but return false
// if it cannot be acquired.
#define taskrequirestaskTryAcquire(self, task) (self)->_->tryAcquire(TaskRequiresTask(self), ComplexTask(task))
// bool taskrequirestaskRelease(TaskRequiresTask* self, ComplexTask* task);
//
// Releases the resource, MUST be paired with acquire and called with the same task used to acquire.
#define taskrequirestaskRelease(self, task) (self)->_->release(TaskRequiresTask(self), ComplexTask(task))
// void taskrequirestaskCancel(TaskRequiresTask* self);
//
// The task is being cancelled and wishes to cascade the cancellation to any dependencies
#define taskrequirestaskCancel(self) (self)->_->cancel(TaskRequiresTask(self))
// bool taskrequirestaskRegisterTask(TaskRequiresTask* self, ComplexTask* task);
//
// Requests that the requirement notify the task when conditions change that may change the state of the
// requirement. The requirement must notify the task by advancing it out of the defer queue. This also registers
// the task with shared resources if needed. Such as registration is a one-shot and is consumed when the
// resource is acquired.
#define taskrequirestaskRegisterTask(self, task) (self)->_->registerTask(TaskRequiresTask(self), ComplexTask(task))

