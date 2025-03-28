#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqworker.h"
#include <cx/thread.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQThreadWorker TQThreadWorker;
typedef struct TQThreadWorker_WeakRef TQThreadWorker_WeakRef;
saDeclarePtr(TQThreadWorker);
saDeclarePtr(TQThreadWorker_WeakRef);

typedef struct TQThreadWorker_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // hooks for derived classes to override if desired
    // worker is starting up
    void (*onStart)(_In_ void* self, _In_ TaskQueue* tq);
    // worker should process tasks (call parent!)
    int64 (*tick)(_In_ void* self, _In_ TaskQueue* tq);
    // worker is shutting down
    void (*onStop)(_In_ void* self, _In_ TaskQueue* tq);
    bool (*startThread)(_In_ void* self, _In_ TaskQueue* tq);
} TQThreadWorker_ClassIf;
extern TQThreadWorker_ClassIf TQThreadWorker_ClassIf_tmpl;

typedef struct TQThreadWorker {
    union {
        TQThreadWorker_ClassIf* _;
        void* _is_TQThreadWorker;
        void* _is_TQWorker;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Thread* thr;        // worker thread
    atomic(ptr) curtask;        // this worker's currently running task
    TQUICallback ui;
    int32 num;        // worker number
    bool shutdown;
} TQThreadWorker;
extern ObjClassInfo TQThreadWorker_clsinfo;
#define TQThreadWorker(inst) ((TQThreadWorker*)(unused_noeval((inst) && &((inst)->_is_TQThreadWorker)), (inst)))
#define TQThreadWorkerNone ((TQThreadWorker*)NULL)

typedef struct TQThreadWorker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQThreadWorker_WeakRef;
        void* _is_TQWorker_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQThreadWorker_WeakRef;
#define TQThreadWorker_WeakRef(inst) ((TQThreadWorker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQThreadWorker_WeakRef)), (inst)))

_objfactory_guaranteed TQThreadWorker* TQThreadWorker_create(int32 num);
// TQThreadWorker* tqthreadworkerCreate(int32 num);
#define tqthreadworkerCreate(num) TQThreadWorker_create(num)

// void tqthreadworkerOnStart(TQThreadWorker* self, TaskQueue* tq);
//
// hooks for derived classes to override if desired
// worker is starting up
#define tqthreadworkerOnStart(self, tq) (self)->_->onStart(TQThreadWorker(self), TaskQueue(tq))
// int64 tqthreadworkerTick(TQThreadWorker* self, TaskQueue* tq);
//
// worker should process tasks (call parent!)
#define tqthreadworkerTick(self, tq) (self)->_->tick(TQThreadWorker(self), TaskQueue(tq))
// void tqthreadworkerOnStop(TQThreadWorker* self, TaskQueue* tq);
//
// worker is shutting down
#define tqthreadworkerOnStop(self, tq) (self)->_->onStop(TQThreadWorker(self), TaskQueue(tq))
// bool tqthreadworkerStartThread(TQThreadWorker* self, TaskQueue* tq);
#define tqthreadworkerStartThread(self, tq) (self)->_->startThread(TQThreadWorker(self), TaskQueue(tq))

