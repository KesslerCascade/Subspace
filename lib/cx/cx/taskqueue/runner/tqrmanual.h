#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqrunner.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQManualWorker TQManualWorker;
typedef struct TQManualWorker_WeakRef TQManualWorker_WeakRef;
typedef struct TQManualRunner TQManualRunner;
typedef struct TQManualRunner_WeakRef TQManualRunner_WeakRef;
saDeclarePtr(TQManualRunner);
saDeclarePtr(TQManualRunner_WeakRef);

typedef struct TQManualRunner_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    int64 (*tick)(_In_ void* self);
    bool (*stop)(_In_ void* self);
} TQManualRunner_ClassIf;
extern TQManualRunner_ClassIf TQManualRunner_ClassIf_tmpl;

typedef struct TQManualRunner {
    union {
        TQManualRunner_ClassIf* _;
        void* _is_TQManualRunner;
        void* _is_TQRunner;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    bool needsUIEvent;
    TaskQueue* tq;
    TQManualWorker* worker;
} TQManualRunner;
extern ObjClassInfo TQManualRunner_clsinfo;
#define TQManualRunner(inst) ((TQManualRunner*)(unused_noeval((inst) && &((inst)->_is_TQManualRunner)), (inst)))
#define TQManualRunnerNone ((TQManualRunner*)NULL)

typedef struct TQManualRunner_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQManualRunner_WeakRef;
        void* _is_TQRunner_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQManualRunner_WeakRef;
#define TQManualRunner_WeakRef(inst) ((TQManualRunner_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQManualRunner_WeakRef)), (inst)))

_objfactory_guaranteed TQManualRunner* TQManualRunner_create();
// TQManualRunner* tqmanualrunnerCreate();
#define tqmanualrunnerCreate() TQManualRunner_create()

// bool tqmanualrunnerStart(TQManualRunner* self, TaskQueue* tq);
#define tqmanualrunnerStart(self, tq) (self)->_->start(TQManualRunner(self), TaskQueue(tq))
// int64 tqmanualrunnerTick(TQManualRunner* self);
#define tqmanualrunnerTick(self) (self)->_->tick(TQManualRunner(self))
// bool tqmanualrunnerStop(TQManualRunner* self);
#define tqmanualrunnerStop(self) (self)->_->stop(TQManualRunner(self))

