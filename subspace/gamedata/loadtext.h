#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "gamedata.h"

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
typedef struct VFSDir VFSDir;
typedef struct TaskControl TaskControl;
typedef struct LoadText LoadText;
typedef struct LoadText_WeakRef LoadText_WeakRef;
saDeclarePtr(LoadText);
saDeclarePtr(LoadText_WeakRef);

typedef struct LoadText_ClassIf {
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
} LoadText_ClassIf;
extern LoadText_ClassIf LoadText_ClassIf_tmpl;

typedef struct LoadText {
    union {
        LoadText_ClassIf* _;
        void* _is_LoadText;
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
    Subspace* ss;
    GameData* data;
} LoadText;
extern ObjClassInfo LoadText_clsinfo;
#define LoadText(inst) ((LoadText*)(unused_noeval((inst) && &((inst)->_is_LoadText)), (inst)))
#define LoadTextNone ((LoadText*)NULL)

typedef struct LoadText_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LoadText_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LoadText_WeakRef;
#define LoadText_WeakRef(inst) ((LoadText_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LoadText_WeakRef)), (inst)))

_objfactory_guaranteed LoadText* LoadText_Create(Subspace* ss, GameData* data);
// LoadText* loadtextCreate(Subspace* ss, GameData* data);
#define loadtextCreate(ss, data) LoadText_Create(ss, GameData(data))

// void loadtextRequireTask(LoadText* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define loadtextRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void loadtextRequireTaskTimeout(LoadText* self, Task* dep, bool failok, int64 timeout);
#define loadtextRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void loadtextRequireResource(LoadText* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define loadtextRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void loadtextRequireResourceTimeout(LoadText* self, TaskResource* res, int64 timeout);
#define loadtextRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void loadtextRequireGate(LoadText* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define loadtextRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void loadtextRequireGateTimeout(LoadText* self, TRGate* gate, int64 timeout);
#define loadtextRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void loadtextRequire(LoadText* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define loadtextRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool loadtextAdvance(LoadText* self);
//
// advance a deferred task to run as soon as possible
#define loadtextAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 loadtextCheckRequires(LoadText* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define loadtextCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void loadtextCancelRequires(LoadText* self);
//
// cascade a task cancellation to any requirements
#define loadtextCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool loadtextAcquireRequires(LoadText* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define loadtextAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool loadtextReleaseRequires(LoadText* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define loadtextReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool loadtext_setState(LoadText* self, uint32 newstate);
#define loadtext_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 loadtextRun(LoadText* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define loadtextRun(self, tq, worker, tcon) (self)->_->run(LoadText(self), TaskQueue(tq), TQWorker(worker), tcon)
// void loadtextRunCancelled(LoadText* self, TaskQueue* tq, TQWorker* worker);
#define loadtextRunCancelled(self, tq, worker) (self)->_->runCancelled(LoadText(self), TaskQueue(tq), TQWorker(worker))
// bool loadtextCancel(LoadText* self);
#define loadtextCancel(self) (self)->_->cancel(LoadText(self))
// bool loadtextReset(LoadText* self);
#define loadtextReset(self) (self)->_->reset(LoadText(self))
// bool loadtextWait(LoadText* self, int64 timeout);
#define loadtextWait(self, timeout) (self)->_->wait(LoadText(self), timeout)
// intptr loadtextCmp(LoadText* self, LoadText* other, uint32 flags);
#define loadtextCmp(self, other, flags) (self)->_->cmp(LoadText(self), other, flags)
// uint32 loadtextHash(LoadText* self, uint32 flags);
#define loadtextHash(self, flags) (self)->_->hash(LoadText(self), flags)

