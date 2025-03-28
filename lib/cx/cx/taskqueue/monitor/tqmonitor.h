#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/taskqueue_shared.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQMonitor TQMonitor;
typedef struct TQMonitor_WeakRef TQMonitor_WeakRef;
saDeclarePtr(TQMonitor);
saDeclarePtr(TQMonitor_WeakRef);

typedef struct TQMonitor_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    int64 (*tick)(_In_ void* self);
    bool (*stop)(_In_ void* self);
} TQMonitor_ClassIf;
extern TQMonitor_ClassIf TQMonitor_ClassIf_tmpl;

typedef struct TQMonitor {
    union {
        TQMonitor_ClassIf* _;
        void* _is_TQMonitor;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    TaskQueue* tq;
    TaskQueueMonitorConfig conf;
} TQMonitor;
extern ObjClassInfo TQMonitor_clsinfo;
#define TQMonitor(inst) ((TQMonitor*)(unused_noeval((inst) && &((inst)->_is_TQMonitor)), (inst)))
#define TQMonitorNone ((TQMonitor*)NULL)

typedef struct TQMonitor_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQMonitor_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQMonitor_WeakRef;
#define TQMonitor_WeakRef(inst) ((TQMonitor_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQMonitor_WeakRef)), (inst)))

// bool tqmonitorStart(TQMonitor* self, TaskQueue* tq);
#define tqmonitorStart(self, tq) (self)->_->start(TQMonitor(self), TaskQueue(tq))
// int64 tqmonitorTick(TQMonitor* self);
#define tqmonitorTick(self) (self)->_->tick(TQMonitor(self))
// bool tqmonitorStop(TQMonitor* self);
#define tqmonitorStop(self) (self)->_->stop(TQMonitor(self))

