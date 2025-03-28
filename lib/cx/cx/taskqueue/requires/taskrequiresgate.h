#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "taskrequires.h"
#include <cx/thread/mutex.h>
#include <cx/taskqueue/task/complextask.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct TaskRequiresGate TaskRequiresGate;
typedef struct TaskRequiresGate_WeakRef TaskRequiresGate_WeakRef;
saDeclarePtr(TRGate);
saDeclarePtr(TRGate_WeakRef);
saDeclarePtr(TaskRequiresGate);
saDeclarePtr(TaskRequiresGate_WeakRef);

typedef struct TRGate_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*open)(_In_ void* self);
    bool (*seal)(_In_ void* self);
    void (*progress)(_In_ void* self);
    bool (*registerTask)(_In_ void* self, ComplexTask* task);
} TRGate_ClassIf;
extern TRGate_ClassIf TRGate_ClassIf_tmpl;

typedef struct TaskRequiresGate_ClassIf {
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
} TaskRequiresGate_ClassIf;
extern TaskRequiresGate_ClassIf TaskRequiresGate_ClassIf_tmpl;

typedef struct TRGate {
    union {
        TRGate_ClassIf* _;
        void* _is_TRGate;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    atomic(uint32) state;
    Mutex _wlmtx;
    sa_ComplexTask _waitlist;
    int64 lastprogress;
} TRGate;
extern ObjClassInfo TRGate_clsinfo;
#define TRGate(inst) ((TRGate*)(unused_noeval((inst) && &((inst)->_is_TRGate)), (inst)))
#define TRGateNone ((TRGate*)NULL)

typedef struct TRGate_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TRGate_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TRGate_WeakRef;
#define TRGate_WeakRef(inst) ((TRGate_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TRGate_WeakRef)), (inst)))

_objfactory_guaranteed TRGate* TRGate_create(_In_opt_ strref name);
// TRGate* trgateCreate(strref name);
#define trgateCreate(name) TRGate_create(name)

// bool trgateOpen(TRGate* self);
#define trgateOpen(self) (self)->_->open(TRGate(self))
// bool trgateSeal(TRGate* self);
#define trgateSeal(self) (self)->_->seal(TRGate(self))
// void trgateProgress(TRGate* self);
#define trgateProgress(self) (self)->_->progress(TRGate(self))
// bool trgateRegisterTask(TRGate* self, ComplexTask* task);
#define trgateRegisterTask(self, task) (self)->_->registerTask(TRGate(self), ComplexTask(task))

typedef struct TaskRequiresGate {
    union {
        TaskRequiresGate_ClassIf* _;
        void* _is_TaskRequiresGate;
        void* _is_TaskRequires;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    int64 expires;        // Time after which this requires is considered timed out and expired, and will fail.
    TRGate* gate;
} TaskRequiresGate;
extern ObjClassInfo TaskRequiresGate_clsinfo;
#define TaskRequiresGate(inst) ((TaskRequiresGate*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresGate)), (inst)))
#define TaskRequiresGateNone ((TaskRequiresGate*)NULL)

typedef struct TaskRequiresGate_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskRequiresGate_WeakRef;
        void* _is_TaskRequires_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskRequiresGate_WeakRef;
#define TaskRequiresGate_WeakRef(inst) ((TaskRequiresGate_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskRequiresGate_WeakRef)), (inst)))

_objfactory_guaranteed TaskRequiresGate* TaskRequiresGate_create(_In_ TRGate* gate);
// TaskRequiresGate* taskrequiresgateCreate(TRGate* gate);
#define taskrequiresgateCreate(gate) TaskRequiresGate_create(TRGate(gate))

// uint32 taskrequiresgateState(TaskRequiresGate* self, ComplexTask* task);
//
// Calculate the current state of the requirement.
#define taskrequiresgateState(self, task) (self)->_->state(TaskRequiresGate(self), ComplexTask(task))
// int64 taskrequiresgateProgress(TaskRequiresGate* self);
//
// If possible, return the last progress timestamp associated with the requirement, or -1 if not applicable.
#define taskrequiresgateProgress(self) (self)->_->progress(TaskRequiresGate(self))
// bool taskrequiresgateTryAcquire(TaskRequiresGate* self, ComplexTask* task);
//
// For requirements that involve resource acquisition, attempts to acquire the exclusive resource on behalf
// of task. This should only be called if state() returns TASK_Requires_Acquire. If called, the caller MUST
// then call release() when the task is finished running.
// Implementions of this function must NOT block -- they should try to acquire a lock but return false
// if it cannot be acquired.
#define taskrequiresgateTryAcquire(self, task) (self)->_->tryAcquire(TaskRequiresGate(self), ComplexTask(task))
// bool taskrequiresgateRelease(TaskRequiresGate* self, ComplexTask* task);
//
// Releases the resource, MUST be paired with acquire and called with the same task used to acquire.
#define taskrequiresgateRelease(self, task) (self)->_->release(TaskRequiresGate(self), ComplexTask(task))
// void taskrequiresgateCancel(TaskRequiresGate* self);
//
// The task is being cancelled and wishes to cascade the cancellation to any dependencies
#define taskrequiresgateCancel(self) (self)->_->cancel(TaskRequiresGate(self))
// bool taskrequiresgateRegisterTask(TaskRequiresGate* self, ComplexTask* task);
//
// Requests that the requirement notify the task when conditions change that may change the state of the
// requirement. The requirement must notify the task by advancing it out of the defer queue. This also registers
// the task with shared resources if needed. Such as registration is a one-shot and is consumed when the
// resource is acquired.
#define taskrequiresgateRegisterTask(self, task) (self)->_->registerTask(TaskRequiresGate(self), ComplexTask(task))

