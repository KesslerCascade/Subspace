#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqrunner.h"
#include <cx/taskqueue/worker/tqthreadworker.h>
#include <cx/thread/rwlock.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQThreadWorker TQThreadWorker;
typedef struct TQThreadWorker_WeakRef TQThreadWorker_WeakRef;
typedef struct TQThreadPoolRunner TQThreadPoolRunner;
typedef struct TQThreadPoolRunner_WeakRef TQThreadPoolRunner_WeakRef;
saDeclarePtr(TQThreadPoolRunner);
saDeclarePtr(TQThreadPoolRunner_WeakRef);

typedef struct TQThreadPoolRunner_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*start)(_In_ void* self, _In_ TaskQueue* tq);
    int64 (*tick)(_In_ void* self);
    bool (*stop)(_In_ void* self);
    bool (*addWorker)(_In_ void* self);
    bool (*removeWorker)(_In_ void* self);
    // worker factory for custom queues to override
    _objfactory_guaranteed TQThreadWorker* (*createWorker)(_In_ void* self, int32 num);
} TQThreadPoolRunner_ClassIf;
extern TQThreadPoolRunner_ClassIf TQThreadPoolRunner_ClassIf_tmpl;

typedef struct TQThreadPoolRunner {
    union {
        TQThreadPoolRunner_ClassIf* _;
        void* _is_TQThreadPoolRunner;
        void* _is_TQRunner;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    bool needsUIEvent;
    TaskQueue* tq;
    TaskQueueThreadPoolConfig conf;
    RWLock workerlock;
    sa_TQThreadWorker workers;
    Event workershutdown;
} TQThreadPoolRunner;
extern ObjClassInfo TQThreadPoolRunner_clsinfo;
#define TQThreadPoolRunner(inst) ((TQThreadPoolRunner*)(unused_noeval((inst) && &((inst)->_is_TQThreadPoolRunner)), (inst)))
#define TQThreadPoolRunnerNone ((TQThreadPoolRunner*)NULL)

typedef struct TQThreadPoolRunner_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQThreadPoolRunner_WeakRef;
        void* _is_TQRunner_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQThreadPoolRunner_WeakRef;
#define TQThreadPoolRunner_WeakRef(inst) ((TQThreadPoolRunner_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQThreadPoolRunner_WeakRef)), (inst)))

_objfactory_guaranteed TQThreadPoolRunner* TQThreadPoolRunner_create(_In_ TaskQueueThreadPoolConfig* config);
// TQThreadPoolRunner* tqthreadpoolrunnerCreate(TaskQueueThreadPoolConfig* config);
#define tqthreadpoolrunnerCreate(config) TQThreadPoolRunner_create(config)

// bool tqthreadpoolrunnerStart(TQThreadPoolRunner* self, TaskQueue* tq);
#define tqthreadpoolrunnerStart(self, tq) (self)->_->start(TQThreadPoolRunner(self), TaskQueue(tq))
// int64 tqthreadpoolrunnerTick(TQThreadPoolRunner* self);
#define tqthreadpoolrunnerTick(self) (self)->_->tick(TQThreadPoolRunner(self))
// bool tqthreadpoolrunnerStop(TQThreadPoolRunner* self);
#define tqthreadpoolrunnerStop(self) (self)->_->stop(TQThreadPoolRunner(self))
// bool tqthreadpoolrunnerAddWorker(TQThreadPoolRunner* self);
#define tqthreadpoolrunnerAddWorker(self) (self)->_->addWorker(TQThreadPoolRunner(self))
// bool tqthreadpoolrunnerRemoveWorker(TQThreadPoolRunner* self);
#define tqthreadpoolrunnerRemoveWorker(self) (self)->_->removeWorker(TQThreadPoolRunner(self))
// TQThreadWorker* tqthreadpoolrunnerCreateWorker(TQThreadPoolRunner* self, int32 num);
//
// worker factory for custom queues to override
#define tqthreadpoolrunnerCreateWorker(self, num) (self)->_->createWorker(TQThreadPoolRunner(self), num)

