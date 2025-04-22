#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "controlclient.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct ControlTask ControlTask;
typedef struct ControlTask_WeakRef ControlTask_WeakRef;
saDeclarePtr(ControlTask);
saDeclarePtr(ControlTask_WeakRef);

typedef struct ControlTask_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
} ControlTask_ClassIf;
extern ControlTask_ClassIf ControlTask_ClassIf_tmpl;

typedef struct ControlTask {
    union {
        ControlTask_ClassIf* _;
        void* _is_ControlTask;
        void* _is_ComplexTask;
        void* _is_Task;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    string name;        // task name to be shown in monitor output
    int64 last;        // the last time this task was moved between queues and/or run
    cchain oncomplete;        // functions that are called when this task has completed
    int64 nextrun;        // next time for this task to run when scheduled
    int64 lastprogress;        // timestamp of last progress change
    Weak(ComplexTaskQueue)* lastq;        // The last queue this task ran on before it was deferred
    sa_TaskRequires _requires;        // list of requirements that must be satisfied
    uint16 flags;        // flags to customize task behavior
    uint16 _intflags;        // internal flags reserved for use by the scheduler
    atomic(uint32) _advcount;        // number of times this task has been advanced
    ControlClient* client;
    ControlMsg* msg;        // message to process (control task becomes owner)
} ControlTask;
extern ObjClassInfo ControlTask_clsinfo;
#define ControlTask(inst) ((ControlTask*)(unused_noeval((inst) && &((inst)->_is_ControlTask)), (inst)))
#define ControlTaskNone ((ControlTask*)NULL)

typedef struct ControlTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ControlTask_WeakRef;
#define ControlTask_WeakRef(inst) ((ControlTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ControlTask_WeakRef)), (inst)))

// void controltaskRequireTask(ControlTask* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define controltaskRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void controltaskRequireTaskTimeout(ControlTask* self, Task* dep, bool failok, int64 timeout);
#define controltaskRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void controltaskRequireResource(ControlTask* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define controltaskRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void controltaskRequireResourceTimeout(ControlTask* self, TaskResource* res, int64 timeout);
#define controltaskRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void controltaskRequireGate(ControlTask* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define controltaskRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void controltaskRequireGateTimeout(ControlTask* self, TRGate* gate, int64 timeout);
#define controltaskRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void controltaskRequire(ControlTask* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define controltaskRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool controltaskAdvance(ControlTask* self);
//
// advance a deferred task to run as soon as possible
#define controltaskAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 controltaskCheckRequires(ControlTask* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define controltaskCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void controltaskCancelRequires(ControlTask* self);
//
// cascade a task cancellation to any requirements
#define controltaskCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool controltaskAcquireRequires(ControlTask* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define controltaskAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool controltaskReleaseRequires(ControlTask* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define controltaskReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool controltask_setState(ControlTask* self, uint32 newstate);
#define controltask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 controltaskRun(ControlTask* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define controltaskRun(self, tq, worker, tcon) (self)->_->run(ControlTask(self), TaskQueue(tq), TQWorker(worker), tcon)
// void controltaskRunCancelled(ControlTask* self, TaskQueue* tq, TQWorker* worker);
#define controltaskRunCancelled(self, tq, worker) (self)->_->runCancelled(ControlTask(self), TaskQueue(tq), TQWorker(worker))
// bool controltaskCancel(ControlTask* self);
#define controltaskCancel(self) (self)->_->cancel(ControlTask(self))
// bool controltaskReset(ControlTask* self);
#define controltaskReset(self) (self)->_->reset(ControlTask(self))
// bool controltaskWait(ControlTask* self, int64 timeout);
#define controltaskWait(self, timeout) (self)->_->wait(ControlTask(self), timeout)
// intptr controltaskCmp(ControlTask* self, ControlTask* other, uint32 flags);
#define controltaskCmp(self, other, flags) (self)->_->cmp(ControlTask(self), other, flags)
// uint32 controltaskHash(ControlTask* self, uint32 flags);
#define controltaskHash(self, flags) (self)->_->hash(ControlTask(self), flags)

