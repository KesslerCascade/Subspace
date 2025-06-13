#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "subspace.h"
#include "logent.h"

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
typedef struct RunInfo RunInfo;
typedef struct RunInfo_WeakRef RunInfo_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct LogRelay LogRelay;
typedef struct LogRelay_WeakRef LogRelay_WeakRef;
typedef struct LogDispatcher LogDispatcher;
typedef struct LogDispatcher_WeakRef LogDispatcher_WeakRef;
saDeclarePtr(LogRelay);
saDeclarePtr(LogRelay_WeakRef);
saDeclarePtr(LogDispatcher);
saDeclarePtr(LogDispatcher_WeakRef);

typedef struct LogSubscriber {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*logNotify)(_In_ void* self, LogEnt* ent, bool replay);
    void (*logReset)(_In_ void* self);
} LogSubscriber;
extern LogSubscriber LogSubscriber_tmpl;

typedef struct LogRelay_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*subscribe)(_In_ void* self, ObjInst* subscriber, _In_opt_ strref id);
    void (*reset)(_In_ void* self);
    bool (*send)(_In_ void* self, LogEnt* ent, bool replay);
} LogRelay_ClassIf;
extern LogRelay_ClassIf LogRelay_ClassIf_tmpl;

typedef struct LogDispatcher_ClassIf {
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
} LogDispatcher_ClassIf;
extern LogDispatcher_ClassIf LogDispatcher_ClassIf_tmpl;

typedef struct LogRelay {
    union {
        LogRelay_ClassIf* _;
        void* _is_LogRelay;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    TRFifo* replayqueue;
    RWLock lock;
    hashtable logsubs;
    hashtable subscribers;
} LogRelay;
extern ObjClassInfo LogRelay_clsinfo;
#define LogRelay(inst) ((LogRelay*)(unused_noeval((inst) && &((inst)->_is_LogRelay)), (inst)))
#define LogRelayNone ((LogRelay*)NULL)

typedef struct LogRelay_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LogRelay_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LogRelay_WeakRef;
#define LogRelay_WeakRef(inst) ((LogRelay_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LogRelay_WeakRef)), (inst)))

_objfactory_guaranteed LogRelay* LogRelay_create(Subspace* ss);
// LogRelay* logrelayCreate(Subspace* ss);
#define logrelayCreate(ss) LogRelay_create(ss)

// bool logrelaySubscribe(LogRelay* self, ObjInst* subscriber, strref id);
#define logrelaySubscribe(self, subscriber, id) (self)->_->subscribe(LogRelay(self), ObjInst(subscriber), id)
// void logrelayReset(LogRelay* self);
#define logrelayReset(self) (self)->_->reset(LogRelay(self))
// bool logrelaySend(LogRelay* self, LogEnt* ent, bool replay);
#define logrelaySend(self, ent, replay) (self)->_->send(LogRelay(self), LogEnt(ent), replay)

typedef struct LogDispatcher {
    union {
        LogDispatcher_ClassIf* _;
        void* _is_LogDispatcher;
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
    ObjInst* subscriber;
    LogEnt* ent;
    bool replay;
} LogDispatcher;
extern ObjClassInfo LogDispatcher_clsinfo;
#define LogDispatcher(inst) ((LogDispatcher*)(unused_noeval((inst) && &((inst)->_is_LogDispatcher)), (inst)))
#define LogDispatcherNone ((LogDispatcher*)NULL)

typedef struct LogDispatcher_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LogDispatcher_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LogDispatcher_WeakRef;
#define LogDispatcher_WeakRef(inst) ((LogDispatcher_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LogDispatcher_WeakRef)), (inst)))

_objfactory_guaranteed LogDispatcher* LogDispatcher_create(ObjInst* subscriber, LogEnt* ent, bool replay);
// LogDispatcher* logdispatcherCreate(ObjInst* subscriber, LogEnt* ent, bool replay);
#define logdispatcherCreate(subscriber, ent, replay) LogDispatcher_create(ObjInst(subscriber), LogEnt(ent), replay)

// void logdispatcherRequireTask(LogDispatcher* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define logdispatcherRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void logdispatcherRequireTaskTimeout(LogDispatcher* self, Task* dep, bool failok, int64 timeout);
#define logdispatcherRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void logdispatcherRequireResource(LogDispatcher* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define logdispatcherRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void logdispatcherRequireResourceTimeout(LogDispatcher* self, TaskResource* res, int64 timeout);
#define logdispatcherRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void logdispatcherRequireGate(LogDispatcher* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define logdispatcherRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void logdispatcherRequireGateTimeout(LogDispatcher* self, TRGate* gate, int64 timeout);
#define logdispatcherRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void logdispatcherRequire(LogDispatcher* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define logdispatcherRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool logdispatcherAdvance(LogDispatcher* self);
//
// advance a deferred task to run as soon as possible
#define logdispatcherAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 logdispatcherCheckRequires(LogDispatcher* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define logdispatcherCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void logdispatcherCancelRequires(LogDispatcher* self);
//
// cascade a task cancellation to any requirements
#define logdispatcherCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool logdispatcherAcquireRequires(LogDispatcher* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define logdispatcherAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool logdispatcherReleaseRequires(LogDispatcher* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define logdispatcherReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool logdispatcher_setState(LogDispatcher* self, uint32 newstate);
#define logdispatcher_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 logdispatcherRun(LogDispatcher* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define logdispatcherRun(self, tq, worker, tcon) (self)->_->run(LogDispatcher(self), TaskQueue(tq), TQWorker(worker), tcon)
// void logdispatcherRunCancelled(LogDispatcher* self, TaskQueue* tq, TQWorker* worker);
#define logdispatcherRunCancelled(self, tq, worker) (self)->_->runCancelled(LogDispatcher(self), TaskQueue(tq), TQWorker(worker))
// bool logdispatcherCancel(LogDispatcher* self);
#define logdispatcherCancel(self) (self)->_->cancel(LogDispatcher(self))
// bool logdispatcherReset(LogDispatcher* self);
#define logdispatcherReset(self) (self)->_->reset(LogDispatcher(self))
// bool logdispatcherWait(LogDispatcher* self, int64 timeout);
#define logdispatcherWait(self, timeout) (self)->_->wait(LogDispatcher(self), timeout)
// intptr logdispatcherCmp(LogDispatcher* self, LogDispatcher* other, uint32 flags);
#define logdispatcherCmp(self, other, flags) (self)->_->cmp(LogDispatcher(self), other, flags)
// uint32 logdispatcherHash(LogDispatcher* self, uint32 flags);
#define logdispatcherHash(self, flags) (self)->_->hash(LogDispatcher(self), flags)

