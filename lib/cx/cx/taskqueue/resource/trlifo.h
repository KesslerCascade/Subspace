#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "taskresource.h"
#include <cx/thread/mutex.h>
#include <cx/taskqueue/task/complextask.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct TRLifo TRLifo;
typedef struct TRLifo_WeakRef TRLifo_WeakRef;
saDeclarePtr(TRLifo);
saDeclarePtr(TRLifo_WeakRef);

typedef struct TRLifo_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // Registers a task with the shared resource. This enables the task to be advanced
    // when the resource is available and reserves a slot for things like FIFO queues.
    // This registration is consumed by a successful acquisition.
    bool (*registerTask)(_In_ void* self, ComplexTask* task);
    // Is it even possible for the given task to try to acquire the resource right now?
    bool (*canAcquire)(_In_ void* self, ComplexTask* task);
    // Try to acquire the resource. State tracking is up to the caller!
    bool (*tryAcquire)(_In_ void* self, ComplexTask* task);
    // Release the resource. State tracking is up to the caller!
    void (*release)(_In_ void* self, ComplexTask* task);
} TRLifo_ClassIf;
extern TRLifo_ClassIf TRLifo_ClassIf_tmpl;

typedef struct TRLifo {
    union {
        TRLifo_ClassIf* _;
        void* _is_TRLifo;
        void* _is_TaskResource;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Mutex _lifomtx;
    ComplexTask* cur;
    sa_ComplexTask _lifo;
} TRLifo;
extern ObjClassInfo TRLifo_clsinfo;
#define TRLifo(inst) ((TRLifo*)(unused_noeval((inst) && &((inst)->_is_TRLifo)), (inst)))
#define TRLifoNone ((TRLifo*)NULL)

typedef struct TRLifo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TRLifo_WeakRef;
        void* _is_TaskResource_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TRLifo_WeakRef;
#define TRLifo_WeakRef(inst) ((TRLifo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TRLifo_WeakRef)), (inst)))

_objfactory_guaranteed TRLifo* TRLifo_create();
// TRLifo* trlifoCreate();
#define trlifoCreate() TRLifo_create()

// bool trlifoRegisterTask(TRLifo* self, ComplexTask* task);
//
// Registers a task with the shared resource. This enables the task to be advanced
// when the resource is available and reserves a slot for things like FIFO queues.
// This registration is consumed by a successful acquisition.
#define trlifoRegisterTask(self, task) (self)->_->registerTask(TRLifo(self), ComplexTask(task))
// bool trlifoCanAcquire(TRLifo* self, ComplexTask* task);
//
// Is it even possible for the given task to try to acquire the resource right now?
#define trlifoCanAcquire(self, task) (self)->_->canAcquire(TRLifo(self), ComplexTask(task))
// bool trlifoTryAcquire(TRLifo* self, ComplexTask* task);
//
// Try to acquire the resource. State tracking is up to the caller!
#define trlifoTryAcquire(self, task) (self)->_->tryAcquire(TRLifo(self), ComplexTask(task))
// void trlifoRelease(TRLifo* self, ComplexTask* task);
//
// Release the resource. State tracking is up to the caller!
#define trlifoRelease(self, task) (self)->_->release(TRLifo(self), ComplexTask(task))

