#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqworker.h"

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQManualWorker TQManualWorker;
typedef struct TQManualWorker_WeakRef TQManualWorker_WeakRef;
saDeclarePtr(TQManualWorker);
saDeclarePtr(TQManualWorker_WeakRef);

typedef struct TQManualWorker_ClassIf {
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
} TQManualWorker_ClassIf;
extern TQManualWorker_ClassIf TQManualWorker_ClassIf_tmpl;

typedef struct TQManualWorker {
    union {
        TQManualWorker_ClassIf* _;
        void* _is_TQManualWorker;
        void* _is_TQWorker;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

} TQManualWorker;
extern ObjClassInfo TQManualWorker_clsinfo;
#define TQManualWorker(inst) ((TQManualWorker*)(unused_noeval((inst) && &((inst)->_is_TQManualWorker)), (inst)))
#define TQManualWorkerNone ((TQManualWorker*)NULL)

typedef struct TQManualWorker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQManualWorker_WeakRef;
        void* _is_TQWorker_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQManualWorker_WeakRef;
#define TQManualWorker_WeakRef(inst) ((TQManualWorker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQManualWorker_WeakRef)), (inst)))

_objfactory_guaranteed TQManualWorker* TQManualWorker_create();
// TQManualWorker* tqmanualworkerCreate();
#define tqmanualworkerCreate() TQManualWorker_create()

// void tqmanualworkerOnStart(TQManualWorker* self, TaskQueue* tq);
//
// hooks for derived classes to override if desired
// worker is starting up
#define tqmanualworkerOnStart(self, tq) (self)->_->onStart(TQManualWorker(self), TaskQueue(tq))
// int64 tqmanualworkerTick(TQManualWorker* self, TaskQueue* tq);
//
// worker should process tasks (call parent!)
#define tqmanualworkerTick(self, tq) (self)->_->tick(TQManualWorker(self), TaskQueue(tq))
// void tqmanualworkerOnStop(TQManualWorker* self, TaskQueue* tq);
//
// worker is shutting down
#define tqmanualworkerOnStop(self, tq) (self)->_->onStop(TQManualWorker(self), TaskQueue(tq))

