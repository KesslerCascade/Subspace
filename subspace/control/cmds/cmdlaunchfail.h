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
typedef struct CmdLaunchFail CmdLaunchFail;
typedef struct CmdLaunchFail_WeakRef CmdLaunchFail_WeakRef;
saDeclarePtr(CmdLaunchFail);
saDeclarePtr(CmdLaunchFail_WeakRef);

typedef struct CmdLaunchFail_ClassIf {
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
} CmdLaunchFail_ClassIf;
extern CmdLaunchFail_ClassIf CmdLaunchFail_ClassIf_tmpl;

typedef struct CmdLaunchFail {
    union {
        CmdLaunchFail_ClassIf* _;
        void* _is_CmdLaunchFail;
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
} CmdLaunchFail;
extern ObjClassInfo CmdLaunchFail_clsinfo;
#define CmdLaunchFail(inst) ((CmdLaunchFail*)(unused_noeval((inst) && &((inst)->_is_CmdLaunchFail)), (inst)))
#define CmdLaunchFailNone ((CmdLaunchFail*)NULL)

typedef struct CmdLaunchFail_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_CmdLaunchFail_WeakRef;
        void* _is_ControlTask_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} CmdLaunchFail_WeakRef;
#define CmdLaunchFail_WeakRef(inst) ((CmdLaunchFail_WeakRef*)(unused_noeval((inst) && &((inst)->_is_CmdLaunchFail_WeakRef)), (inst)))

void CmdLaunchFail_register(ControlServer* svr);
// void cmdlaunchfailRegister(ControlServer* svr);
#define cmdlaunchfailRegister(svr) CmdLaunchFail_register(ControlServer(svr))

// void cmdlaunchfailRequireTask(CmdLaunchFail* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define cmdlaunchfailRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void cmdlaunchfailRequireTaskTimeout(CmdLaunchFail* self, Task* dep, bool failok, int64 timeout);
#define cmdlaunchfailRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void cmdlaunchfailRequireResource(CmdLaunchFail* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define cmdlaunchfailRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void cmdlaunchfailRequireResourceTimeout(CmdLaunchFail* self, TaskResource* res, int64 timeout);
#define cmdlaunchfailRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void cmdlaunchfailRequireGate(CmdLaunchFail* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define cmdlaunchfailRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void cmdlaunchfailRequireGateTimeout(CmdLaunchFail* self, TRGate* gate, int64 timeout);
#define cmdlaunchfailRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void cmdlaunchfailRequire(CmdLaunchFail* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define cmdlaunchfailRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool cmdlaunchfailAdvance(CmdLaunchFail* self);
//
// advance a deferred task to run as soon as possible
#define cmdlaunchfailAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 cmdlaunchfailCheckRequires(CmdLaunchFail* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define cmdlaunchfailCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void cmdlaunchfailCancelRequires(CmdLaunchFail* self);
//
// cascade a task cancellation to any requirements
#define cmdlaunchfailCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool cmdlaunchfailAcquireRequires(CmdLaunchFail* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define cmdlaunchfailAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool cmdlaunchfailReleaseRequires(CmdLaunchFail* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define cmdlaunchfailReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool cmdlaunchfail_setState(CmdLaunchFail* self, uint32 newstate);
#define cmdlaunchfail_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 cmdlaunchfailRun(CmdLaunchFail* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define cmdlaunchfailRun(self, tq, worker, tcon) (self)->_->run(CmdLaunchFail(self), TaskQueue(tq), TQWorker(worker), tcon)
// void cmdlaunchfailRunCancelled(CmdLaunchFail* self, TaskQueue* tq, TQWorker* worker);
#define cmdlaunchfailRunCancelled(self, tq, worker) (self)->_->runCancelled(CmdLaunchFail(self), TaskQueue(tq), TQWorker(worker))
// bool cmdlaunchfailCancel(CmdLaunchFail* self);
#define cmdlaunchfailCancel(self) (self)->_->cancel(CmdLaunchFail(self))
// bool cmdlaunchfailReset(CmdLaunchFail* self);
#define cmdlaunchfailReset(self) (self)->_->reset(CmdLaunchFail(self))
// bool cmdlaunchfailWait(CmdLaunchFail* self, int64 timeout);
#define cmdlaunchfailWait(self, timeout) (self)->_->wait(CmdLaunchFail(self), timeout)
// intptr cmdlaunchfailCmp(CmdLaunchFail* self, CmdLaunchFail* other, uint32 flags);
#define cmdlaunchfailCmp(self, other, flags) (self)->_->cmp(CmdLaunchFail(self), other, flags)
// uint32 cmdlaunchfailHash(CmdLaunchFail* self, uint32 flags);
#define cmdlaunchfailHash(self, flags) (self)->_->hash(CmdLaunchFail(self), flags)

