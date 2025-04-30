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
typedef struct CmdGameState CmdGameState;
typedef struct CmdGameState_WeakRef CmdGameState_WeakRef;
saDeclarePtr(CmdGameState);
saDeclarePtr(CmdGameState_WeakRef);

typedef struct CmdGameState_ClassIf {
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
} CmdGameState_ClassIf;
extern CmdGameState_ClassIf CmdGameState_ClassIf_tmpl;

typedef struct CmdGameState {
    union {
        CmdGameState_ClassIf* _;
        void* _is_CmdGameState;
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
} CmdGameState;
extern ObjClassInfo CmdGameState_clsinfo;
#define CmdGameState(inst) ((CmdGameState*)(unused_noeval((inst) && &((inst)->_is_CmdGameState)), (inst)))
#define CmdGameStateNone ((CmdGameState*)NULL)

typedef struct CmdGameState_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdGameState_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdGameState_WeakRef;
#define CmdGameState_WeakRef(inst) ((CmdGameState_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdGameState_WeakRef)), (inst)))

void CmdGameState_register(ControlServer* svr);
// void cmdgamestateRegister(ControlServer* svr);
#define cmdgamestateRegister(svr) CmdGameState_register(ControlServer(svr))

// void cmdgamestateRequireTask(CmdGameState* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdgamestateRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdgamestateRequireTaskTimeout(CmdGameState* self, Task* dep, bool failok, int64 timeout);
#define cmdgamestateRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdgamestateRequireResource(CmdGameState* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdgamestateRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdgamestateRequireResourceTimeout(CmdGameState* self, TaskResource* res, int64 timeout);
#define cmdgamestateRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdgamestateRequireGate(CmdGameState* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdgamestateRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdgamestateRequireGateTimeout(CmdGameState* self, TRGate* gate, int64 timeout);
#define cmdgamestateRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdgamestateRequire(CmdGameState* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdgamestateRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdgamestateAdvance(CmdGameState* self);
//
// advance a deferred task to run as soon as possible
#define cmdgamestateAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdgamestateCheckRequires(CmdGameState* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdgamestateCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdgamestateCancelRequires(CmdGameState* self);
//
// cascade a task cancellation to any requirements
#define cmdgamestateCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdgamestateAcquireRequires(CmdGameState* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdgamestateAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdgamestateReleaseRequires(CmdGameState* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdgamestateReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdgamestate_setState(CmdGameState* self, uint32 newstate);
#define cmdgamestate_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdgamestateRun(CmdGameState* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdgamestateRun(self, tq, worker, tcon) (self)->_->run(CmdGameState(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdgamestateRunCancelled(CmdGameState* self, TaskQueue* tq, TQWorker* worker);
#define cmdgamestateRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdGameState(self), TaskQueue(tq), TQWorker(worker))
// bool cmdgamestateCancel(CmdGameState* self);
#define cmdgamestateCancel(self) (self)->_->cancel(CmdGameState(self))
// bool cmdgamestateReset(CmdGameState* self);
#define cmdgamestateReset(self) (self)->_->reset(CmdGameState(self))
// bool cmdgamestateWait(CmdGameState* self, int64 timeout);
#define cmdgamestateWait(self, timeout) (self)->_->wait(CmdGameState(self), timeout)
// intptr cmdgamestateCmp(CmdGameState* self, CmdGameState* other, uint32 flags);
#define cmdgamestateCmp(self, other, flags) (self)->_->cmp(CmdGameState(self), other, flags)
// uint32 cmdgamestateHash(CmdGameState* self, uint32 flags);
#define cmdgamestateHash(self, flags) (self)->_->hash(CmdGameState(self), flags)

