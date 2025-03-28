#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/taskqueue_shared.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQManager TQManager;
typedef struct TQManager_WeakRef TQManager_WeakRef;
saDeclarePtr(TQManager);
saDeclarePtr(TQManager_WeakRef);

#define MAX_MANAGER_INTERVAL (timeS(10))

typedef struct TQManager_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    bool (*stop)(_In_ void* self);
    void (*notify)(_In_ void* self, bool wakeup);
    // for in-worker managers, this is called BEFORE a task is run by the worker
    void (*pretask)(_In_ void* self);
    int64 (*tick)(_In_ void* self);
} TQManager_ClassIf;
extern TQManager_ClassIf TQManager_ClassIf_tmpl;

typedef struct TQManager {
    union {
        TQManager_ClassIf* _;
        void* _is_TQManager;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    TaskQueue* tq;
    bool needsWorkerTick;        // true if this manager needs to be ticked by the worker
} TQManager;
extern ObjClassInfo TQManager_clsinfo;
#define TQManager(inst) ((TQManager*)(unused_noeval((inst) && &((inst)->_is_TQManager)), (inst)))
#define TQManagerNone ((TQManager*)NULL)

typedef struct TQManager_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQManager_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQManager_WeakRef;
#define TQManager_WeakRef(inst) ((TQManager_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQManager_WeakRef)), (inst)))

// bool tqmanagerStart(TQManager* self, TaskQueue* tq);
#define tqmanagerStart(self, tq) (self)->_->start(TQManager(self), TaskQueue(tq))
// bool tqmanagerStop(TQManager* self);
#define tqmanagerStop(self) (self)->_->stop(TQManager(self))
// void tqmanagerNotify(TQManager* self, bool wakeup);
#define tqmanagerNotify(self, wakeup) (self)->_->notify(TQManager(self), wakeup)
// void tqmanagerPretask(TQManager* self);
//
// for in-worker managers, this is called BEFORE a task is run by the worker
#define tqmanagerPretask(self) (self)->_->pretask(TQManager(self))
// int64 tqmanagerTick(TQManager* self);
#define tqmanagerTick(self) (self)->_->tick(TQManager(self))

