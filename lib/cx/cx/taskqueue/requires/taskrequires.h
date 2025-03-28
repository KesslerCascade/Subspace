#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskRequires TaskRequires;
typedef struct TaskRequires_WeakRef TaskRequires_WeakRef;
saDeclarePtr(TaskRequires);
saDeclarePtr(TaskRequires_WeakRef);

enum TaskRequiresStateEnum {
    TASK_Requires_Wait = 0,       // The requirement is not currently satisfied, but may be sometime in the future
    TASK_Requires_Ok,             // The requirement is satisfied for the moment, but it may change in the future
    TASK_Requires_Ok_Permanent,   // The requirement is satisfied and will always be satisfied -- it may be removed
    TASK_Requires_Fail_Permanent, // The requirement is not satisfied, and cannot ever be satisfied
    TASK_Requires_Acquire,        // The requirement will be satisfied if and only if the tryAcquire method returns true
};

typedef struct TaskRequires_ClassIf {
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
} TaskRequires_ClassIf;
extern TaskRequires_ClassIf TaskRequires_ClassIf_tmpl;

typedef struct TaskRequires {
    union {
        TaskRequires_ClassIf* _;
        void* _is_TaskRequires;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 expires;        // Time after which this requires is considered timed out and expired, and will fail.
} TaskRequires;
extern ObjClassInfo TaskRequires_clsinfo;
#define TaskRequires(inst) ((TaskRequires*)(unused_noeval((inst) && &((inst)->_is_TaskRequires)), (inst)))
#define TaskRequiresNone ((TaskRequires*)NULL)

typedef struct TaskRequires_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskRequires_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskRequires_WeakRef;
#define TaskRequires_WeakRef(inst) ((TaskRequires_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskRequires_WeakRef)), (inst)))

// uint32 taskrequiresState(TaskRequires* self, ComplexTask* task);
//
// Calculate the current state of the requirement.
#define taskrequiresState(self, task) (self)->_->state(TaskRequires(self), ComplexTask(task))
// int64 taskrequiresProgress(TaskRequires* self);
//
// If possible, return the last progress timestamp associated with the requirement, or -1 if not applicable.
#define taskrequiresProgress(self) (self)->_->progress(TaskRequires(self))
// bool taskrequiresTryAcquire(TaskRequires* self, ComplexTask* task);
//
// For requirements that involve resource acquisition, attempts to acquire the exclusive resource on behalf
// of task. This should only be called if state() returns TASK_Requires_Acquire. If called, the caller MUST
// then call release() when the task is finished running.
// Implementions of this function must NOT block -- they should try to acquire a lock but return false
// if it cannot be acquired.
#define taskrequiresTryAcquire(self, task) (self)->_->tryAcquire(TaskRequires(self), ComplexTask(task))
// bool taskrequiresRelease(TaskRequires* self, ComplexTask* task);
//
// Releases the resource, MUST be paired with acquire and called with the same task used to acquire.
#define taskrequiresRelease(self, task) (self)->_->release(TaskRequires(self), ComplexTask(task))
// void taskrequiresCancel(TaskRequires* self);
//
// The task is being cancelled and wishes to cascade the cancellation to any dependencies
#define taskrequiresCancel(self) (self)->_->cancel(TaskRequires(self))
// bool taskrequiresRegisterTask(TaskRequires* self, ComplexTask* task);
//
// Requests that the requirement notify the task when conditions change that may change the state of the
// requirement. The requirement must notify the task by advancing it out of the defer queue. This also registers
// the task with shared resources if needed. Such as registration is a one-shot and is consumed when the
// resource is acquired.
#define taskrequiresRegisterTask(self, task) (self)->_->registerTask(TaskRequires(self), ComplexTask(task))

