#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqmanager.h"
#include <cx/thread/mutex.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQManualManager TQManualManager;
typedef struct TQManualManager_WeakRef TQManualManager_WeakRef;
saDeclarePtr(TQManualManager);
saDeclarePtr(TQManualManager_WeakRef);

typedef struct TQManualManager_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    bool (*stop)(_In_ void* self);
    void (*notify)(_In_ void* self, bool wakeup);
    // for in-worker managers, this is called BEFORE a task is run by the worker
    void (*pretask)(_In_ void* self);
    int64 (*tick)(_In_ void* self);
} TQManualManager_ClassIf;
extern TQManualManager_ClassIf TQManualManager_ClassIf_tmpl;

typedef struct TQManualManager {
    union {
        TQManualManager_ClassIf* _;
        void* _is_TQManualManager;
        void* _is_TQManager;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    TaskQueue* tq;
    bool needsWorkerTick;        // true if this manager needs to be ticked by the worker
    Mutex mgrlock;        // one worker can run the manager at a time
    atomic(bool) needrun;        // manager needs to be run ASAP 
} TQManualManager;
extern ObjClassInfo TQManualManager_clsinfo;
#define TQManualManager(inst) ((TQManualManager*)(unused_noeval((inst) && &((inst)->_is_TQManualManager)), (inst)))
#define TQManualManagerNone ((TQManualManager*)NULL)

typedef struct TQManualManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQManualManager_WeakRef;
        void* _is_TQManager_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQManualManager_WeakRef;
#define TQManualManager_WeakRef(inst) ((TQManualManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQManualManager_WeakRef)), (inst)))

_objfactory_guaranteed TQManualManager* TQManualManager_create();
// TQManualManager* tqmanualmanagerCreate();
#define tqmanualmanagerCreate() TQManualManager_create()

// bool tqmanualmanagerStart(TQManualManager* self, TaskQueue* tq);
#define tqmanualmanagerStart(self, tq) (self)->_->start(TQManualManager(self), TaskQueue(tq))
// bool tqmanualmanagerStop(TQManualManager* self);
#define tqmanualmanagerStop(self) (self)->_->stop(TQManualManager(self))
// void tqmanualmanagerNotify(TQManualManager* self, bool wakeup);
#define tqmanualmanagerNotify(self, wakeup) (self)->_->notify(TQManualManager(self), wakeup)
// void tqmanualmanagerPretask(TQManualManager* self);
//
// for in-worker managers, this is called BEFORE a task is run by the worker
#define tqmanualmanagerPretask(self) (self)->_->pretask(TQManualManager(self))
// int64 tqmanualmanagerTick(TQManualManager* self);
#define tqmanualmanagerTick(self) (self)->_->tick(TQManualManager(self))

