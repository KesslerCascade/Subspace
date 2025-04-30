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
typedef struct CmdFeatureState CmdFeatureState;
typedef struct CmdFeatureState_WeakRef CmdFeatureState_WeakRef;
saDeclarePtr(CmdFeatureState);
saDeclarePtr(CmdFeatureState_WeakRef);

typedef struct CmdFeatureState_ClassIf {
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
} CmdFeatureState_ClassIf;
extern CmdFeatureState_ClassIf CmdFeatureState_ClassIf_tmpl;

typedef struct CmdFeatureState {
    union {
        CmdFeatureState_ClassIf* _;
        void* _is_CmdFeatureState;
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
} CmdFeatureState;
extern ObjClassInfo CmdFeatureState_clsinfo;
#define CmdFeatureState(inst) ((CmdFeatureState*)(unused_noeval((inst) && &((inst)->_is_CmdFeatureState)), (inst)))
#define CmdFeatureStateNone ((CmdFeatureState*)NULL)

typedef struct CmdFeatureState_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdFeatureState_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdFeatureState_WeakRef;
#define CmdFeatureState_WeakRef(inst) ((CmdFeatureState_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdFeatureState_WeakRef)), (inst)))

void CmdFeatureState_register(ControlServer* svr);
// void cmdfeaturestateRegister(ControlServer* svr);
#define cmdfeaturestateRegister(svr) CmdFeatureState_register(ControlServer(svr))

// void cmdfeaturestateRequireTask(CmdFeatureState* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdfeaturestateRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdfeaturestateRequireTaskTimeout(CmdFeatureState* self, Task* dep, bool failok, int64 timeout);
#define cmdfeaturestateRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdfeaturestateRequireResource(CmdFeatureState* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdfeaturestateRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdfeaturestateRequireResourceTimeout(CmdFeatureState* self, TaskResource* res, int64 timeout);
#define cmdfeaturestateRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdfeaturestateRequireGate(CmdFeatureState* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdfeaturestateRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdfeaturestateRequireGateTimeout(CmdFeatureState* self, TRGate* gate, int64 timeout);
#define cmdfeaturestateRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdfeaturestateRequire(CmdFeatureState* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdfeaturestateRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdfeaturestateAdvance(CmdFeatureState* self);
//
// advance a deferred task to run as soon as possible
#define cmdfeaturestateAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdfeaturestateCheckRequires(CmdFeatureState* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdfeaturestateCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdfeaturestateCancelRequires(CmdFeatureState* self);
//
// cascade a task cancellation to any requirements
#define cmdfeaturestateCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdfeaturestateAcquireRequires(CmdFeatureState* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdfeaturestateAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdfeaturestateReleaseRequires(CmdFeatureState* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdfeaturestateReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdfeaturestate_setState(CmdFeatureState* self, uint32 newstate);
#define cmdfeaturestate_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdfeaturestateRun(CmdFeatureState* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdfeaturestateRun(self, tq, worker, tcon) (self)->_->run(CmdFeatureState(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdfeaturestateRunCancelled(CmdFeatureState* self, TaskQueue* tq, TQWorker* worker);
#define cmdfeaturestateRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdFeatureState(self), TaskQueue(tq), TQWorker(worker))
// bool cmdfeaturestateCancel(CmdFeatureState* self);
#define cmdfeaturestateCancel(self) (self)->_->cancel(CmdFeatureState(self))
// bool cmdfeaturestateReset(CmdFeatureState* self);
#define cmdfeaturestateReset(self) (self)->_->reset(CmdFeatureState(self))
// bool cmdfeaturestateWait(CmdFeatureState* self, int64 timeout);
#define cmdfeaturestateWait(self, timeout) (self)->_->wait(CmdFeatureState(self), timeout)
// intptr cmdfeaturestateCmp(CmdFeatureState* self, CmdFeatureState* other, uint32 flags);
#define cmdfeaturestateCmp(self, other, flags) (self)->_->cmp(CmdFeatureState(self), other, flags)
// uint32 cmdfeaturestateHash(CmdFeatureState* self, uint32 flags);
#define cmdfeaturestateHash(self, flags) (self)->_->hash(CmdFeatureState(self), flags)

