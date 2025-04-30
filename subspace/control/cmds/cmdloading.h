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
typedef struct ControlServer ControlServer;
typedef struct ControlServer_WeakRef ControlServer_WeakRef;
typedef struct GameInst GameInst;
typedef struct GameInst_WeakRef GameInst_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct CmdLoading CmdLoading;
typedef struct CmdLoading_WeakRef CmdLoading_WeakRef;
saDeclarePtr(CmdLoading);
saDeclarePtr(CmdLoading_WeakRef);

typedef struct CmdLoading_ClassIf {
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
} CmdLoading_ClassIf;
extern CmdLoading_ClassIf CmdLoading_ClassIf_tmpl;

typedef struct CmdLoading {
    union {
        CmdLoading_ClassIf* _;
        void* _is_CmdLoading;
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
} CmdLoading;
extern ObjClassInfo CmdLoading_clsinfo;
#define CmdLoading(inst) ((CmdLoading*)(unused_noeval((inst) && &((inst)->_is_CmdLoading)), (inst)))
#define CmdLoadingNone ((CmdLoading*)NULL)

typedef struct CmdLoading_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdLoading_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdLoading_WeakRef;
#define CmdLoading_WeakRef(inst) ((CmdLoading_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdLoading_WeakRef)), (inst)))

void CmdLoading_register(ControlServer* svr);
// void cmdloadingRegister(ControlServer* svr);
#define cmdloadingRegister(svr) CmdLoading_register(ControlServer(svr))

// void cmdloadingRequireTask(CmdLoading* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdloadingRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdloadingRequireTaskTimeout(CmdLoading* self, Task* dep, bool failok, int64 timeout);
#define cmdloadingRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdloadingRequireResource(CmdLoading* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdloadingRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdloadingRequireResourceTimeout(CmdLoading* self, TaskResource* res, int64 timeout);
#define cmdloadingRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdloadingRequireGate(CmdLoading* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdloadingRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdloadingRequireGateTimeout(CmdLoading* self, TRGate* gate, int64 timeout);
#define cmdloadingRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdloadingRequire(CmdLoading* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdloadingRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdloadingAdvance(CmdLoading* self);
//
// advance a deferred task to run as soon as possible
#define cmdloadingAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdloadingCheckRequires(CmdLoading* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdloadingCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdloadingCancelRequires(CmdLoading* self);
//
// cascade a task cancellation to any requirements
#define cmdloadingCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdloadingAcquireRequires(CmdLoading* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdloadingAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdloadingReleaseRequires(CmdLoading* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdloadingReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdloading_setState(CmdLoading* self, uint32 newstate);
#define cmdloading_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdloadingRun(CmdLoading* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdloadingRun(self, tq, worker, tcon) (self)->_->run(CmdLoading(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdloadingRunCancelled(CmdLoading* self, TaskQueue* tq, TQWorker* worker);
#define cmdloadingRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdLoading(self), TaskQueue(tq), TQWorker(worker))
// bool cmdloadingCancel(CmdLoading* self);
#define cmdloadingCancel(self) (self)->_->cancel(CmdLoading(self))
// bool cmdloadingReset(CmdLoading* self);
#define cmdloadingReset(self) (self)->_->reset(CmdLoading(self))
// bool cmdloadingWait(CmdLoading* self, int64 timeout);
#define cmdloadingWait(self, timeout) (self)->_->wait(CmdLoading(self), timeout)
// intptr cmdloadingCmp(CmdLoading* self, CmdLoading* other, uint32 flags);
#define cmdloadingCmp(self, other, flags) (self)->_->cmp(CmdLoading(self), other, flags)
// uint32 cmdloadingHash(CmdLoading* self, uint32 flags);
#define cmdloadingHash(self, flags) (self)->_->hash(CmdLoading(self), flags)

