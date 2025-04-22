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
typedef struct CmdGameStart CmdGameStart;
typedef struct CmdGameStart_WeakRef CmdGameStart_WeakRef;
saDeclarePtr(CmdGameStart);
saDeclarePtr(CmdGameStart_WeakRef);

typedef struct CmdGameStart_ClassIf {
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
} CmdGameStart_ClassIf;
extern CmdGameStart_ClassIf CmdGameStart_ClassIf_tmpl;

typedef struct CmdGameStart {
    union {
        CmdGameStart_ClassIf* _;
        void* _is_CmdGameStart;
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
} CmdGameStart;
extern ObjClassInfo CmdGameStart_clsinfo;
#define CmdGameStart(inst) ((CmdGameStart*)(unused_noeval((inst) && &((inst)->_is_CmdGameStart)), (inst)))
#define CmdGameStartNone ((CmdGameStart*)NULL)

typedef struct CmdGameStart_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdGameStart_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdGameStart_WeakRef;
#define CmdGameStart_WeakRef(inst) ((CmdGameStart_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdGameStart_WeakRef)), (inst)))

void CmdGameStart_register();
// void cmdgamestartRegister();
#define cmdgamestartRegister() CmdGameStart_register()

// void cmdgamestartRequireTask(CmdGameStart* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdgamestartRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdgamestartRequireTaskTimeout(CmdGameStart* self, Task* dep, bool failok, int64 timeout);
#define cmdgamestartRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdgamestartRequireResource(CmdGameStart* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdgamestartRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdgamestartRequireResourceTimeout(CmdGameStart* self, TaskResource* res, int64 timeout);
#define cmdgamestartRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdgamestartRequireGate(CmdGameStart* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdgamestartRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdgamestartRequireGateTimeout(CmdGameStart* self, TRGate* gate, int64 timeout);
#define cmdgamestartRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdgamestartRequire(CmdGameStart* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdgamestartRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdgamestartAdvance(CmdGameStart* self);
//
// advance a deferred task to run as soon as possible
#define cmdgamestartAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdgamestartCheckRequires(CmdGameStart* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdgamestartCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdgamestartCancelRequires(CmdGameStart* self);
//
// cascade a task cancellation to any requirements
#define cmdgamestartCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdgamestartAcquireRequires(CmdGameStart* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdgamestartAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdgamestartReleaseRequires(CmdGameStart* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdgamestartReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdgamestart_setState(CmdGameStart* self, uint32 newstate);
#define cmdgamestart_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdgamestartRun(CmdGameStart* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdgamestartRun(self, tq, worker, tcon) (self)->_->run(CmdGameStart(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdgamestartRunCancelled(CmdGameStart* self, TaskQueue* tq, TQWorker* worker);
#define cmdgamestartRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdGameStart(self), TaskQueue(tq), TQWorker(worker))
// bool cmdgamestartCancel(CmdGameStart* self);
#define cmdgamestartCancel(self) (self)->_->cancel(CmdGameStart(self))
// bool cmdgamestartReset(CmdGameStart* self);
#define cmdgamestartReset(self) (self)->_->reset(CmdGameStart(self))
// bool cmdgamestartWait(CmdGameStart* self, int64 timeout);
#define cmdgamestartWait(self, timeout) (self)->_->wait(CmdGameStart(self), timeout)
// intptr cmdgamestartCmp(CmdGameStart* self, CmdGameStart* other, uint32 flags);
#define cmdgamestartCmp(self, other, flags) (self)->_->cmp(CmdGameStart(self), other, flags)
// uint32 cmdgamestartHash(CmdGameStart* self, uint32 flags);
#define cmdgamestartHash(self, flags) (self)->_->hash(CmdGameStart(self), flags)

