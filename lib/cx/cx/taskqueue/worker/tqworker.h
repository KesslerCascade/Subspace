#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/taskqueue_shared.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
saDeclarePtr(TQWorker);
saDeclarePtr(TQWorker_WeakRef);

typedef struct TQWorker_ClassIf {
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
} TQWorker_ClassIf;
extern TQWorker_ClassIf TQWorker_ClassIf_tmpl;

typedef struct TQWorker {
    union {
        TQWorker_ClassIf* _;
        void* _is_TQWorker;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

} TQWorker;
extern ObjClassInfo TQWorker_clsinfo;
#define TQWorker(inst) ((TQWorker*)(unused_noeval((inst) && &((inst)->_is_TQWorker)), (inst)))
#define TQWorkerNone ((TQWorker*)NULL)

typedef struct TQWorker_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TQWorker_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TQWorker_WeakRef;
#define TQWorker_WeakRef(inst) ((TQWorker_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TQWorker_WeakRef)), (inst)))

// void tqworkerOnStart(TQWorker* self, TaskQueue* tq);
//
// hooks for derived classes to override if desired
// worker is starting up
#define tqworkerOnStart(self, tq) (self)->_->onStart(TQWorker(self), TaskQueue(tq))
// int64 tqworkerTick(TQWorker* self, TaskQueue* tq);
//
// worker should process tasks (call parent!)
#define tqworkerTick(self, tq) (self)->_->tick(TQWorker(self), TaskQueue(tq))
// void tqworkerOnStop(TQWorker* self, TaskQueue* tq);
//
// worker is shutting down
#define tqworkerOnStop(self, tq) (self)->_->onStop(TQWorker(self), TaskQueue(tq))

