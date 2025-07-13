#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "subspace.h"
#include <cx/fs/vfsobj.h>
#include <cx/taskqueue.h>

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
typedef struct GameData GameData;
typedef struct GameData_WeakRef GameData_WeakRef;
typedef struct GameDataLoader GameDataLoader;
typedef struct GameDataLoader_WeakRef GameDataLoader_WeakRef;
saDeclarePtr(GameData);
saDeclarePtr(GameData_WeakRef);
saDeclarePtr(GameDataLoader);
saDeclarePtr(GameDataLoader_WeakRef);

typedef enum GameDataState {
    GD_Init,
    GD_Loading,
    GD_Available,
    GD_Error
} GameDataState;

typedef struct GameData_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*load)(_In_ void* self, Subspace* ss, VFS* parentvfs, _In_opt_ strref ftldir);
    GameDataState (*state)(_In_ void* self);
    // make a task wait for data
    void (*wait)(_In_ void* self, ComplexTask* task);
    strref (*getText)(_In_ void* self, _In_opt_ strref key);
    strref (*getTextD)(_In_ void* self, _In_opt_ strref key, _In_opt_ strref def);
} GameData_ClassIf;
extern GameData_ClassIf GameData_ClassIf_tmpl;

typedef struct GameDataLoader_ClassIf {
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
} GameDataLoader_ClassIf;
extern GameDataLoader_ClassIf GameDataLoader_ClassIf_tmpl;

typedef struct GameData {
    union {
        GameData_ClassIf* _;
        void* _is_GameData;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    VFS* ftldat;
    RWLock lock;
    GameDataState _state;
    TRGate* loaded;
    hashtable text;        // language translations
} GameData;
extern ObjClassInfo GameData_clsinfo;
#define GameData(inst) ((GameData*)(unused_noeval((inst) && &((inst)->_is_GameData)), (inst)))
#define GameDataNone ((GameData*)NULL)

typedef struct GameData_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameData_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameData_WeakRef;
#define GameData_WeakRef(inst) ((GameData_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameData_WeakRef)), (inst)))

_objfactory_guaranteed GameData* GameData_create();
// GameData* gamedataCreate();
#define gamedataCreate() GameData_create()

// bool gamedataLoad(GameData* self, Subspace* ss, VFS* parentvfs, strref ftldir);
#define gamedataLoad(self, ss, parentvfs, ftldir) (self)->_->load(GameData(self), ss, VFS(parentvfs), ftldir)
// GameDataState gamedataState(GameData* self);
#define gamedataState(self) (self)->_->state(GameData(self))
// void gamedataWait(GameData* self, ComplexTask* task);
//
// make a task wait for data
#define gamedataWait(self, task) (self)->_->wait(GameData(self), ComplexTask(task))
// strref gamedataGetText(GameData* self, strref key);
#define gamedataGetText(self, key) (self)->_->getText(GameData(self), key)
// strref gamedataGetTextD(GameData* self, strref key, strref def);
#define gamedataGetTextD(self, key, def) (self)->_->getTextD(GameData(self), key, def)

typedef struct GameDataLoader {
    union {
        GameDataLoader_ClassIf* _;
        void* _is_GameDataLoader;
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
} GameDataLoader;
extern ObjClassInfo GameDataLoader_clsinfo;
#define GameDataLoader(inst) ((GameDataLoader*)(unused_noeval((inst) && &((inst)->_is_GameDataLoader)), (inst)))
#define GameDataLoaderNone ((GameDataLoader*)NULL)

typedef struct GameDataLoader_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_GameDataLoader_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} GameDataLoader_WeakRef;
#define GameDataLoader_WeakRef(inst) ((GameDataLoader_WeakRef*)(unused_noeval((inst) && &((inst)->_is_GameDataLoader_WeakRef)), (inst)))

_objfactory_guaranteed GameDataLoader* GameDataLoader_create(Subspace* ss, GameData* data);
// GameDataLoader* gamedataloaderCreate(Subspace* ss, GameData* data);
#define gamedataloaderCreate(ss, data) GameDataLoader_create(ss, GameData(data))

// void gamedataloaderRequireTask(GameDataLoader* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define gamedataloaderRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void gamedataloaderRequireTaskTimeout(GameDataLoader* self, Task* dep, bool failok, int64 timeout);
#define gamedataloaderRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void gamedataloaderRequireResource(GameDataLoader* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define gamedataloaderRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void gamedataloaderRequireResourceTimeout(GameDataLoader* self, TaskResource* res, int64 timeout);
#define gamedataloaderRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void gamedataloaderRequireGate(GameDataLoader* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define gamedataloaderRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void gamedataloaderRequireGateTimeout(GameDataLoader* self, TRGate* gate, int64 timeout);
#define gamedataloaderRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void gamedataloaderRequire(GameDataLoader* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define gamedataloaderRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool gamedataloaderAdvance(GameDataLoader* self);
//
// advance a deferred task to run as soon as possible
#define gamedataloaderAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 gamedataloaderCheckRequires(GameDataLoader* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define gamedataloaderCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void gamedataloaderCancelRequires(GameDataLoader* self);
//
// cascade a task cancellation to any requirements
#define gamedataloaderCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool gamedataloaderAcquireRequires(GameDataLoader* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define gamedataloaderAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool gamedataloaderReleaseRequires(GameDataLoader* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define gamedataloaderReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool gamedataloader_setState(GameDataLoader* self, uint32 newstate);
#define gamedataloader_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 gamedataloaderRun(GameDataLoader* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define gamedataloaderRun(self, tq, worker, tcon) (self)->_->run(GameDataLoader(self), TaskQueue(tq), TQWorker(worker), tcon)
// void gamedataloaderRunCancelled(GameDataLoader* self, TaskQueue* tq, TQWorker* worker);
#define gamedataloaderRunCancelled(self, tq, worker) (self)->_->runCancelled(GameDataLoader(self), TaskQueue(tq), TQWorker(worker))
// bool gamedataloaderCancel(GameDataLoader* self);
#define gamedataloaderCancel(self) (self)->_->cancel(GameDataLoader(self))
// bool gamedataloaderReset(GameDataLoader* self);
#define gamedataloaderReset(self) (self)->_->reset(GameDataLoader(self))
// bool gamedataloaderWait(GameDataLoader* self, int64 timeout);
#define gamedataloaderWait(self, timeout) (self)->_->wait(GameDataLoader(self), timeout)
// intptr gamedataloaderCmp(GameDataLoader* self, GameDataLoader* other, uint32 flags);
#define gamedataloaderCmp(self, other, flags) (self)->_->cmp(GameDataLoader(self), other, flags)
// uint32 gamedataloaderHash(GameDataLoader* self, uint32 flags);
#define gamedataloaderHash(self, flags) (self)->_->hash(GameDataLoader(self), flags)

