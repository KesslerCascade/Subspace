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
typedef struct SubscriberInfo SubscriberInfo;
typedef struct SubscriberInfo_WeakRef SubscriberInfo_WeakRef;
typedef struct LogRelay LogRelay;
typedef struct LogRelay_WeakRef LogRelay_WeakRef;
typedef struct LogDispatch LogDispatch;
typedef struct LogDispatch_WeakRef LogDispatch_WeakRef;
saDeclarePtr(SubscriberInfo);
saDeclarePtr(SubscriberInfo_WeakRef);
saDeclarePtr(LogRelay);
saDeclarePtr(LogRelay_WeakRef);
saDeclarePtr(LogDispatch);
saDeclarePtr(LogDispatch_WeakRef);

typedef struct LogSubscriber {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*logNotify)(_In_ void* self, LogEnt* ent, bool replay);
    void (*logReset)(_In_ void* self);
    void (*logReplayComplete)(_In_ void* self);
} LogSubscriber;
extern LogSubscriber LogSubscriber_tmpl;

typedef struct SubscriberInfo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
} SubscriberInfo_ClassIf;
extern SubscriberInfo_ClassIf SubscriberInfo_ClassIf_tmpl;

typedef struct LogRelay_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*subscribe)(_In_ void* self, ObjInst* subscriber, _In_opt_ strref id);
    bool (*subscribeUI)(_In_ void* self, ObjInst* subscriber, _In_opt_ strref id);
    void (*reset)(_In_ void* self);
    void (*replayComplete)(_In_ void* self);
    bool (*send)(_In_ void* self, LogEnt* ent, bool replay);
} LogRelay_ClassIf;
extern LogRelay_ClassIf LogRelay_ClassIf_tmpl;

typedef struct LogDispatch_ClassIf {
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
} LogDispatch_ClassIf;
extern LogDispatch_ClassIf LogDispatch_ClassIf_tmpl;

typedef struct SubscriberInfo {
    union {
        SubscriberInfo_ClassIf* _;
        void* _is_SubscriberInfo;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Weak(ObjInst)* subscriber;
    bool ui;
} SubscriberInfo;
extern ObjClassInfo SubscriberInfo_clsinfo;
#define SubscriberInfo(inst) ((SubscriberInfo*)(unused_noeval((inst) && &((inst)->_is_SubscriberInfo)), (inst)))
#define SubscriberInfoNone ((SubscriberInfo*)NULL)

typedef struct SubscriberInfo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SubscriberInfo_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SubscriberInfo_WeakRef;
#define SubscriberInfo_WeakRef(inst) ((SubscriberInfo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SubscriberInfo_WeakRef)), (inst)))

_objfactory_guaranteed SubscriberInfo* SubscriberInfo_create(ObjInst* subscriber, bool ui);
// SubscriberInfo* subscriberinfoCreate(ObjInst* subscriber, bool ui);
#define subscriberinfoCreate(subscriber, ui) SubscriberInfo_create(ObjInst(subscriber), ui)

// intptr subscriberinfoCmp(SubscriberInfo* self, SubscriberInfo* other, uint32 flags);
#define subscriberinfoCmp(self, other, flags) (self)->_->cmp(SubscriberInfo(self), other, flags)
// uint32 subscriberinfoHash(SubscriberInfo* self, uint32 flags);
#define subscriberinfoHash(self, flags) (self)->_->hash(SubscriberInfo(self), flags)

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
// bool logrelaySubscribeUI(LogRelay* self, ObjInst* subscriber, strref id);
#define logrelaySubscribeUI(self, subscriber, id) (self)->_->subscribeUI(LogRelay(self), ObjInst(subscriber), id)
// void logrelayReset(LogRelay* self);
#define logrelayReset(self) (self)->_->reset(LogRelay(self))
// void logrelayReplayComplete(LogRelay* self);
#define logrelayReplayComplete(self) (self)->_->replayComplete(LogRelay(self))
// bool logrelaySend(LogRelay* self, LogEnt* ent, bool replay);
#define logrelaySend(self, ent, replay) (self)->_->send(LogRelay(self), LogEnt(ent), replay)

typedef struct LogDispatch {
    union {
        LogDispatch_ClassIf* _;
        void* _is_LogDispatch;
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
    bool reset;
    bool complete;
} LogDispatch;
extern ObjClassInfo LogDispatch_clsinfo;
#define LogDispatch(inst) ((LogDispatch*)(unused_noeval((inst) && &((inst)->_is_LogDispatch)), (inst)))
#define LogDispatchNone ((LogDispatch*)NULL)

typedef struct LogDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LogDispatch_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LogDispatch_WeakRef;
#define LogDispatch_WeakRef(inst) ((LogDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LogDispatch_WeakRef)), (inst)))

_objfactory_guaranteed LogDispatch* LogDispatch_create(ObjInst* subscriber, LogEnt* ent, bool replay);
// LogDispatch* logdispatchCreate(ObjInst* subscriber, LogEnt* ent, bool replay);
#define logdispatchCreate(subscriber, ent, replay) LogDispatch_create(ObjInst(subscriber), LogEnt(ent), replay)

_objfactory_guaranteed LogDispatch* LogDispatch_createReplay(ObjInst* subscriber, bool reset, bool complete);
// LogDispatch* logdispatchCreateReplay(ObjInst* subscriber, bool reset, bool complete);
#define logdispatchCreateReplay(subscriber, reset, complete) LogDispatch_createReplay(ObjInst(subscriber), reset, complete)

// void logdispatchRequireTask(LogDispatch* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define logdispatchRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void logdispatchRequireTaskTimeout(LogDispatch* self, Task* dep, bool failok, int64 timeout);
#define logdispatchRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void logdispatchRequireResource(LogDispatch* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define logdispatchRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void logdispatchRequireResourceTimeout(LogDispatch* self, TaskResource* res, int64 timeout);
#define logdispatchRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void logdispatchRequireGate(LogDispatch* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define logdispatchRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void logdispatchRequireGateTimeout(LogDispatch* self, TRGate* gate, int64 timeout);
#define logdispatchRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void logdispatchRequire(LogDispatch* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define logdispatchRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool logdispatchAdvance(LogDispatch* self);
//
// advance a deferred task to run as soon as possible
#define logdispatchAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 logdispatchCheckRequires(LogDispatch* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define logdispatchCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void logdispatchCancelRequires(LogDispatch* self);
//
// cascade a task cancellation to any requirements
#define logdispatchCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool logdispatchAcquireRequires(LogDispatch* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define logdispatchAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool logdispatchReleaseRequires(LogDispatch* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define logdispatchReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool logdispatch_setState(LogDispatch* self, uint32 newstate);
#define logdispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 logdispatchRun(LogDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define logdispatchRun(self, tq, worker, tcon) (self)->_->run(LogDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void logdispatchRunCancelled(LogDispatch* self, TaskQueue* tq, TQWorker* worker);
#define logdispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(LogDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool logdispatchCancel(LogDispatch* self);
#define logdispatchCancel(self) (self)->_->cancel(LogDispatch(self))
// bool logdispatchReset(LogDispatch* self);
#define logdispatchReset(self) (self)->_->reset(LogDispatch(self))
// bool logdispatchWait(LogDispatch* self, int64 timeout);
#define logdispatchWait(self, timeout) (self)->_->wait(LogDispatch(self), timeout)
// intptr logdispatchCmp(LogDispatch* self, LogDispatch* other, uint32 flags);
#define logdispatchCmp(self, other, flags) (self)->_->cmp(LogDispatch(self), other, flags)
// uint32 logdispatchHash(LogDispatch* self, uint32 flags);
#define logdispatchHash(self, flags) (self)->_->hash(LogDispatch(self), flags)

