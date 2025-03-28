#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqmthreadpool.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQThreadPoolRunner TQThreadPoolRunner;
typedef struct TQThreadPoolRunner_WeakRef TQThreadPoolRunner_WeakRef;
typedef struct TQDedicatedManager TQDedicatedManager;
typedef struct TQDedicatedManager_WeakRef TQDedicatedManager_WeakRef;
saDeclarePtr(TQDedicatedManager);
saDeclarePtr(TQDedicatedManager_WeakRef);

typedef struct TQDedicatedManager_ClassIf {
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
} TQDedicatedManager_ClassIf;
extern TQDedicatedManager_ClassIf TQDedicatedManager_ClassIf_tmpl;

typedef struct TQDedicatedManager {
    union {
        TQDedicatedManager_ClassIf* _;
        void* _is_TQDedicatedManager;
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
    Thread* mgrthread;
    Event mgrnotify;
} TQDedicatedManager;
extern ObjClassInfo TQDedicatedManager_clsinfo;
#define TQDedicatedManager(inst) ((TQDedicatedManager*)(unused_noeval((inst) && &((inst)->_is_TQDedicatedManager)), (inst)))
#define TQDedicatedManagerNone ((TQDedicatedManager*)NULL)

typedef struct TQDedicatedManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQDedicatedManager_WeakRef;
        void* _is_TQThreadPoolManager_WeakRef;
        void* _is_TQManager_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQDedicatedManager_WeakRef;
#define TQDedicatedManager_WeakRef(inst) ((TQDedicatedManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQDedicatedManager_WeakRef)), (inst)))

_objfactory_guaranteed TQDedicatedManager* TQDedicatedManager_create();
// TQDedicatedManager* tqdedicatedmanagerCreate();
#define tqdedicatedmanagerCreate() TQDedicatedManager_create()

// bool tqdedicatedmanagerStart(TQDedicatedManager* self, TaskQueue* tq);
#define tqdedicatedmanagerStart(self, tq) (self)->_->start(TQDedicatedManager(self), TaskQueue(tq))
// bool tqdedicatedmanagerStop(TQDedicatedManager* self);
#define tqdedicatedmanagerStop(self) (self)->_->stop(TQDedicatedManager(self))
// void tqdedicatedmanagerNotify(TQDedicatedManager* self, bool wakeup);
#define tqdedicatedmanagerNotify(self, wakeup) (self)->_->notify(TQDedicatedManager(self), wakeup)
// void tqdedicatedmanagerPretask(TQDedicatedManager* self);
//
// for in-worker managers, this is called BEFORE a task is run by the worker
#define tqdedicatedmanagerPretask(self) (self)->_->pretask(TQDedicatedManager(self))
// int64 tqdedicatedmanagerTick(TQDedicatedManager* self);
#define tqdedicatedmanagerTick(self) (self)->_->tick(TQDedicatedManager(self))
// void tqdedicatedmanagerUpdatePoolSize(TQDedicatedManager* self);
#define tqdedicatedmanagerUpdatePoolSize(self) (self)->_->updatePoolSize(TQDedicatedManager(self))

