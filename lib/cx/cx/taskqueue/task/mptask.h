#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "complextask.h"

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
typedef struct MultiphaseTask MultiphaseTask;
typedef struct MultiphaseTask_WeakRef MultiphaseTask_WeakRef;
saDeclarePtr(MultiphaseTask);
saDeclarePtr(MultiphaseTask_WeakRef);

enum MultiphaseTaskFlagsEnum {
    MPTASK_Greedy = 0x10
};

typedef uint32 (*MPTPhaseFunc)(void *self, TaskQueue* tq, TQWorker* worker, TaskControl *tcon);
saDeclare(MPTPhaseFunc);
#define mptaskAddPhases(self, parr) mptask_addPhases(self, sizeof(parr) / sizeof((parr)[0]), (parr), false)
#define mptaskAddFailPhases(self, parr) mptask_addPhases(self, sizeof(parr) / sizeof((parr)[0]), (parr), true)

typedef struct MultiphaseTask_ClassIf {
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
    // Called once all phases (including fail phases) have completed. May be overridden to perform
    // additional cleanup or change the final result.
    uint32 (*finish)(_In_ void* self, uint32 result, TaskControl* tcon);
} MultiphaseTask_ClassIf;
extern MultiphaseTask_ClassIf MultiphaseTask_ClassIf_tmpl;

typedef struct MultiphaseTask {
    union {
        MultiphaseTask_ClassIf* _;
        void* _is_MultiphaseTask;
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
    sa_MPTPhaseFunc phases;        // phases that are run during normal task execution
    sa_MPTPhaseFunc failphases;        // phases that only run on the event of failure
    uint32 _phase;
    bool _fail;
} MultiphaseTask;
extern ObjClassInfo MultiphaseTask_clsinfo;
#define MultiphaseTask(inst) ((MultiphaseTask*)(unused_noeval((inst) && &((inst)->_is_MultiphaseTask)), (inst)))
#define MultiphaseTaskNone ((MultiphaseTask*)NULL)

typedef struct MultiphaseTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_MultiphaseTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} MultiphaseTask_WeakRef;
#define MultiphaseTask_WeakRef(inst) ((MultiphaseTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_MultiphaseTask_WeakRef)), (inst)))

void MultiphaseTask__addPhases(_In_ MultiphaseTask* self, int32 num, MPTPhaseFunc parr[], bool fail);
// void mptask_addPhases(MultiphaseTask* self, int32 num, MPTPhaseFunc parr[], bool fail);
//
// Adds phases from a static array.
#define mptask_addPhases(self, num, parr, fail) MultiphaseTask__addPhases(MultiphaseTask(self), num, parr, fail)

// void mptaskRequireTask(MultiphaseTask* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define mptaskRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void mptaskRequireTaskTimeout(MultiphaseTask* self, Task* dep, bool failok, int64 timeout);
#define mptaskRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void mptaskRequireResource(MultiphaseTask* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define mptaskRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void mptaskRequireResourceTimeout(MultiphaseTask* self, TaskResource* res, int64 timeout);
#define mptaskRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void mptaskRequireGate(MultiphaseTask* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define mptaskRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void mptaskRequireGateTimeout(MultiphaseTask* self, TRGate* gate, int64 timeout);
#define mptaskRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void mptaskRequire(MultiphaseTask* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define mptaskRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool mptaskAdvance(MultiphaseTask* self);
//
// advance a deferred task to run as soon as possible
#define mptaskAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 mptaskCheckRequires(MultiphaseTask* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define mptaskCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void mptaskCancelRequires(MultiphaseTask* self);
//
// cascade a task cancellation to any requirements
#define mptaskCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool mptaskAcquireRequires(MultiphaseTask* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define mptaskAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool mptaskReleaseRequires(MultiphaseTask* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define mptaskReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool mptask_setState(MultiphaseTask* self, uint32 newstate);
#define mptask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 mptaskRun(MultiphaseTask* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define mptaskRun(self, tq, worker, tcon) (self)->_->run(MultiphaseTask(self), TaskQueue(tq), TQWorker(worker), tcon)
// void mptaskRunCancelled(MultiphaseTask* self, TaskQueue* tq, TQWorker* worker);
#define mptaskRunCancelled(self, tq, worker) (self)->_->runCancelled(MultiphaseTask(self), TaskQueue(tq), TQWorker(worker))
// bool mptaskCancel(MultiphaseTask* self);
#define mptaskCancel(self) (self)->_->cancel(MultiphaseTask(self))
// bool mptaskReset(MultiphaseTask* self);
#define mptaskReset(self) (self)->_->reset(MultiphaseTask(self))
// bool mptaskWait(MultiphaseTask* self, int64 timeout);
#define mptaskWait(self, timeout) (self)->_->wait(MultiphaseTask(self), timeout)
// intptr mptaskCmp(MultiphaseTask* self, MultiphaseTask* other, uint32 flags);
#define mptaskCmp(self, other, flags) (self)->_->cmp(MultiphaseTask(self), other, flags)
// uint32 mptaskHash(MultiphaseTask* self, uint32 flags);
#define mptaskHash(self, flags) (self)->_->hash(MultiphaseTask(self), flags)
// uint32 mptaskFinish(MultiphaseTask* self, uint32 result, TaskControl* tcon);
//
// Called once all phases (including fail phases) have completed. May be overridden to perform
// additional cleanup or change the final result.
#define mptaskFinish(self, result, tcon) (self)->_->finish(MultiphaseTask(self), result, tcon)

