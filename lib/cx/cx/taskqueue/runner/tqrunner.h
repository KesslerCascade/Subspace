#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/taskqueue_shared.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQRunner TQRunner;
typedef struct TQRunner_WeakRef TQRunner_WeakRef;
saDeclarePtr(TQRunner);
saDeclarePtr(TQRunner_WeakRef);

typedef struct TQRunner_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    int64 (*tick)(_In_ void* self);
    bool (*stop)(_In_ void* self);
} TQRunner_ClassIf;
extern TQRunner_ClassIf TQRunner_ClassIf_tmpl;

typedef struct TQRunner {
    union {
        TQRunner_ClassIf* _;
        void* _is_TQRunner;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    bool needsUIEvent;
    TaskQueue* tq;
} TQRunner;
extern ObjClassInfo TQRunner_clsinfo;
#define TQRunner(inst) ((TQRunner*)(unused_noeval((inst) && &((inst)->_is_TQRunner)), (inst)))
#define TQRunnerNone ((TQRunner*)NULL)

typedef struct TQRunner_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQRunner_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQRunner_WeakRef;
#define TQRunner_WeakRef(inst) ((TQRunner_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQRunner_WeakRef)), (inst)))

// bool tqrunnerStart(TQRunner* self, TaskQueue* tq);
#define tqrunnerStart(self, tq) (self)->_->start(TQRunner(self), TaskQueue(tq))
// int64 tqrunnerTick(TQRunner* self);
#define tqrunnerTick(self) (self)->_->tick(TQRunner(self))
// bool tqrunnerStop(TQRunner* self);
#define tqrunnerStop(self) (self)->_->stop(TQRunner(self))

