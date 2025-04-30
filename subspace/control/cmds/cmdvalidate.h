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
typedef struct CmdValidate CmdValidate;
typedef struct CmdValidate_WeakRef CmdValidate_WeakRef;
saDeclarePtr(CmdValidate);
saDeclarePtr(CmdValidate_WeakRef);

typedef struct CmdValidate_ClassIf {
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
} CmdValidate_ClassIf;
extern CmdValidate_ClassIf CmdValidate_ClassIf_tmpl;

typedef struct CmdValidate {
    union {
        CmdValidate_ClassIf* _;
        void* _is_CmdValidate;
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
} CmdValidate;
extern ObjClassInfo CmdValidate_clsinfo;
#define CmdValidate(inst) ((CmdValidate*)(unused_noeval((inst) && &((inst)->_is_CmdValidate)), (inst)))
#define CmdValidateNone ((CmdValidate*)NULL)

typedef struct CmdValidate_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdValidate_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdValidate_WeakRef;
#define CmdValidate_WeakRef(inst) ((CmdValidate_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdValidate_WeakRef)), (inst)))

void CmdValidate_register(ControlServer* svr);
// void cmdvalidateRegister(ControlServer* svr);
#define cmdvalidateRegister(svr) CmdValidate_register(ControlServer(svr))

// void cmdvalidateRequireTask(CmdValidate* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdvalidateRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdvalidateRequireTaskTimeout(CmdValidate* self, Task* dep, bool failok, int64 timeout);
#define cmdvalidateRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdvalidateRequireResource(CmdValidate* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdvalidateRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdvalidateRequireResourceTimeout(CmdValidate* self, TaskResource* res, int64 timeout);
#define cmdvalidateRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdvalidateRequireGate(CmdValidate* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdvalidateRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdvalidateRequireGateTimeout(CmdValidate* self, TRGate* gate, int64 timeout);
#define cmdvalidateRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdvalidateRequire(CmdValidate* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdvalidateRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdvalidateAdvance(CmdValidate* self);
//
// advance a deferred task to run as soon as possible
#define cmdvalidateAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdvalidateCheckRequires(CmdValidate* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdvalidateCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdvalidateCancelRequires(CmdValidate* self);
//
// cascade a task cancellation to any requirements
#define cmdvalidateCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdvalidateAcquireRequires(CmdValidate* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdvalidateAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdvalidateReleaseRequires(CmdValidate* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdvalidateReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdvalidate_setState(CmdValidate* self, uint32 newstate);
#define cmdvalidate_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdvalidateRun(CmdValidate* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdvalidateRun(self, tq, worker, tcon) (self)->_->run(CmdValidate(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdvalidateRunCancelled(CmdValidate* self, TaskQueue* tq, TQWorker* worker);
#define cmdvalidateRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdValidate(self), TaskQueue(tq), TQWorker(worker))
// bool cmdvalidateCancel(CmdValidate* self);
#define cmdvalidateCancel(self) (self)->_->cancel(CmdValidate(self))
// bool cmdvalidateReset(CmdValidate* self);
#define cmdvalidateReset(self) (self)->_->reset(CmdValidate(self))
// bool cmdvalidateWait(CmdValidate* self, int64 timeout);
#define cmdvalidateWait(self, timeout) (self)->_->wait(CmdValidate(self), timeout)
// intptr cmdvalidateCmp(CmdValidate* self, CmdValidate* other, uint32 flags);
#define cmdvalidateCmp(self, other, flags) (self)->_->cmp(CmdValidate(self), other, flags)
// uint32 cmdvalidateHash(CmdValidate* self, uint32 flags);
#define cmdvalidateHash(self, flags) (self)->_->hash(CmdValidate(self), flags)

