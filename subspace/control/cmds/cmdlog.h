#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "control/controltask.h"

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
typedef struct CmdLog CmdLog;
typedef struct CmdLog_WeakRef CmdLog_WeakRef;
saDeclarePtr(CmdLog);
saDeclarePtr(CmdLog_WeakRef);

typedef struct CmdLog_ClassIf {
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
} CmdLog_ClassIf;
extern CmdLog_ClassIf CmdLog_ClassIf_tmpl;

typedef struct CmdLog {
    union {
        CmdLog_ClassIf* _;
        void* _is_CmdLog;
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
} CmdLog;
extern ObjClassInfo CmdLog_clsinfo;
#define CmdLog(inst) ((CmdLog*)(unused_noeval((inst) && &((inst)->_is_CmdLog)), (inst)))
#define CmdLogNone ((CmdLog*)NULL)

typedef struct CmdLog_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdLog_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdLog_WeakRef;
#define CmdLog_WeakRef(inst) ((CmdLog_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdLog_WeakRef)), (inst)))

void CmdLog_register(ControlServer* svr);
// void cmdlogRegister(ControlServer* svr);
#define cmdlogRegister(svr) CmdLog_register(ControlServer(svr))

// void cmdlogRequireTask(CmdLog* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdlogRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdlogRequireTaskTimeout(CmdLog* self, Task* dep, bool failok, int64 timeout);
#define cmdlogRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdlogRequireResource(CmdLog* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdlogRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdlogRequireResourceTimeout(CmdLog* self, TaskResource* res, int64 timeout);
#define cmdlogRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdlogRequireGate(CmdLog* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdlogRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdlogRequireGateTimeout(CmdLog* self, TRGate* gate, int64 timeout);
#define cmdlogRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdlogRequire(CmdLog* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdlogRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdlogAdvance(CmdLog* self);
//
// advance a deferred task to run as soon as possible
#define cmdlogAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdlogCheckRequires(CmdLog* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdlogCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdlogCancelRequires(CmdLog* self);
//
// cascade a task cancellation to any requirements
#define cmdlogCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdlogAcquireRequires(CmdLog* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdlogAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdlogReleaseRequires(CmdLog* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdlogReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdlog_setState(CmdLog* self, uint32 newstate);
#define cmdlog_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdlogRun(CmdLog* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdlogRun(self, tq, worker, tcon) (self)->_->run(CmdLog(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdlogRunCancelled(CmdLog* self, TaskQueue* tq, TQWorker* worker);
#define cmdlogRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdLog(self), TaskQueue(tq), TQWorker(worker))
// bool cmdlogCancel(CmdLog* self);
#define cmdlogCancel(self) (self)->_->cancel(CmdLog(self))
// bool cmdlogReset(CmdLog* self);
#define cmdlogReset(self) (self)->_->reset(CmdLog(self))
// bool cmdlogWait(CmdLog* self, int64 timeout);
#define cmdlogWait(self, timeout) (self)->_->wait(CmdLog(self), timeout)
// intptr cmdlogCmp(CmdLog* self, CmdLog* other, uint32 flags);
#define cmdlogCmp(self, other, flags) (self)->_->cmp(CmdLog(self), other, flags)
// uint32 cmdlogHash(CmdLog* self, uint32 flags);
#define cmdlogHash(self, flags) (self)->_->hash(CmdLog(self), flags)

