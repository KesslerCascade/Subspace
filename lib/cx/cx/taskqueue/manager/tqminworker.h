#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqmthreadpool.h"
#include <cx/thread/mutex.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQThreadPoolRunner TQThreadPoolRunner;
typedef struct TQThreadPoolRunner_WeakRef TQThreadPoolRunner_WeakRef;
typedef struct TQInWorkerManager TQInWorkerManager;
typedef struct TQInWorkerManager_WeakRef TQInWorkerManager_WeakRef;
saDeclarePtr(TQInWorkerManager);
saDeclarePtr(TQInWorkerManager_WeakRef);

typedef struct TQInWorkerManager_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    bool (*stop)(_In_ void* self);
    void (*notify)(_In_ void* self, bool wakeup);
    // for in-worker managers, this is called BEFORE a task is run by the worker
    void (*pretask)(_In_ void* self);
    int64 (*tick)(_In_ void* self);
    void (*updatePoolSize)(_In_ void* self);
} TQInWorkerManager_ClassIf;
extern TQInWorkerManager_ClassIf TQInWorkerManager_ClassIf_tmpl;

typedef struct TQInWorkerManager {
    union {
        TQInWorkerManager_ClassIf* _;
        void* _is_TQInWorkerManager;
        void* _is_TQThreadPoolManager;
        void* _is_TQManager;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    TaskQueue* tq;
    bool needsWorkerTick;        // true if this manager needs to be ticked by the worker
    TQThreadPoolRunner* runner;
    uint32 lastcount;
    int64 lastop;
    int64 idlestart;
    Mutex mgrlock;        // one worker can run the manager at a time
    atomic(bool) needrun;        // manager needs to be run ASAP 
} TQInWorkerManager;
extern ObjClassInfo TQInWorkerManager_clsinfo;
#define TQInWorkerManager(inst) ((TQInWorkerManager*)(unused_noeval((inst) && &((inst)->_is_TQInWorkerManager)), (inst)))
#define TQInWorkerManagerNone ((TQInWorkerManager*)NULL)

typedef struct TQInWorkerManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQInWorkerManager_WeakRef;
        void* _is_TQThreadPoolManager_WeakRef;
        void* _is_TQManager_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQInWorkerManager_WeakRef;
#define TQInWorkerManager_WeakRef(inst) ((TQInWorkerManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQInWorkerManager_WeakRef)), (inst)))

_objfactory_guaranteed TQInWorkerManager* TQInWorkerManager_create();
// TQInWorkerManager* tqinworkermanagerCreate();
#define tqinworkermanagerCreate() TQInWorkerManager_create()

// bool tqinworkermanagerStart(TQInWorkerManager* self, TaskQueue* tq);
#define tqinworkermanagerStart(self, tq) (self)->_->start(TQInWorkerManager(self), TaskQueue(tq))
// bool tqinworkermanagerStop(TQInWorkerManager* self);
#define tqinworkermanagerStop(self) (self)->_->stop(TQInWorkerManager(self))
// void tqinworkermanagerNotify(TQInWorkerManager* self, bool wakeup);
#define tqinworkermanagerNotify(self, wakeup) (self)->_->notify(TQInWorkerManager(self), wakeup)
// void tqinworkermanagerPretask(TQInWorkerManager* self);
//
// for in-worker managers, this is called BEFORE a task is run by the worker
#define tqinworkermanagerPretask(self) (self)->_->pretask(TQInWorkerManager(self))
// int64 tqinworkermanagerTick(TQInWorkerManager* self);
#define tqinworkermanagerTick(self) (self)->_->tick(TQInWorkerManager(self))
// void tqinworkermanagerUpdatePoolSize(TQInWorkerManager* self);
#define tqinworkermanagerUpdatePoolSize(self) (self)->_->updatePoolSize(TQInWorkerManager(self))

