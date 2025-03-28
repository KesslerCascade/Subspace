#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqmonitor.h"
#include <cx/thread/mutex.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQThreadPoolMonitor TQThreadPoolMonitor;
typedef struct TQThreadPoolMonitor_WeakRef TQThreadPoolMonitor_WeakRef;
saDeclarePtr(TQThreadPoolMonitor);
saDeclarePtr(TQThreadPoolMonitor_WeakRef);

typedef struct TQThreadPoolMonitor_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    int64 (*tick)(_In_ void* self);
    bool (*stop)(_In_ void* self);
} TQThreadPoolMonitor_ClassIf;
extern TQThreadPoolMonitor_ClassIf TQThreadPoolMonitor_ClassIf_tmpl;

typedef struct TQThreadPoolMonitor {
    union {
        TQThreadPoolMonitor_ClassIf* _;
        void* _is_TQThreadPoolMonitor;
        void* _is_TQMonitor;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    TaskQueue* tq;
    TaskQueueMonitorConfig conf;
    Mutex monitorlock;
    int64 lastrun;
    int64 lastwarn;
} TQThreadPoolMonitor;
extern ObjClassInfo TQThreadPoolMonitor_clsinfo;
#define TQThreadPoolMonitor(inst) ((TQThreadPoolMonitor*)(unused_noeval((inst) && &((inst)->_is_TQThreadPoolMonitor)), (inst)))
#define TQThreadPoolMonitorNone ((TQThreadPoolMonitor*)NULL)

typedef struct TQThreadPoolMonitor_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQThreadPoolMonitor_WeakRef;
        void* _is_TQMonitor_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQThreadPoolMonitor_WeakRef;
#define TQThreadPoolMonitor_WeakRef(inst) ((TQThreadPoolMonitor_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQThreadPoolMonitor_WeakRef)), (inst)))

_objfactory_guaranteed TQThreadPoolMonitor* TQThreadPoolMonitor_create(_In_ TaskQueueMonitorConfig* config);
// TQThreadPoolMonitor* tqthreadpoolmonitorCreate(TaskQueueMonitorConfig* config);
#define tqthreadpoolmonitorCreate(config) TQThreadPoolMonitor_create(config)

// bool tqthreadpoolmonitorStart(TQThreadPoolMonitor* self, TaskQueue* tq);
#define tqthreadpoolmonitorStart(self, tq) (self)->_->start(TQThreadPoolMonitor(self), TaskQueue(tq))
// int64 tqthreadpoolmonitorTick(TQThreadPoolMonitor* self);
#define tqthreadpoolmonitorTick(self) (self)->_->tick(TQThreadPoolMonitor(self))
// bool tqthreadpoolmonitorStop(TQThreadPoolMonitor* self);
#define tqthreadpoolmonitorStop(self) (self)->_->stop(TQThreadPoolMonitor(self))

