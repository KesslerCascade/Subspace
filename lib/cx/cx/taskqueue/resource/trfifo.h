#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "taskresource.h"
#include <cx/thread/mutex.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRFifoNode TRFifoNode;
typedef struct TRFifo TRFifo;
typedef struct TRFifo_WeakRef TRFifo_WeakRef;
saDeclarePtr(TRFifo);
saDeclarePtr(TRFifo_WeakRef);

typedef struct TRFifoNode {
    TRFifoNode *next;
    ComplexTask *task;
} TRFifoNode;

typedef struct TRFifo_ClassIf {
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
} TRFifo_ClassIf;
extern TRFifo_ClassIf TRFifo_ClassIf_tmpl;

typedef struct TRFifo {
    union {
        TRFifo_ClassIf* _;
        void* _is_TRFifo;
        void* _is_TaskResource;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Mutex _fifomtx;
    ComplexTask* cur;
    TRFifoNode* head;
    TRFifoNode* tail;
} TRFifo;
extern ObjClassInfo TRFifo_clsinfo;
#define TRFifo(inst) ((TRFifo*)(unused_noeval((inst) && &((inst)->_is_TRFifo)), (inst)))
#define TRFifoNone ((TRFifo*)NULL)

typedef struct TRFifo_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TRFifo_WeakRef;
        void* _is_TaskResource_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TRFifo_WeakRef;
#define TRFifo_WeakRef(inst) ((TRFifo_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TRFifo_WeakRef)), (inst)))

_objfactory_guaranteed TRFifo* TRFifo_create();
// TRFifo* trfifoCreate();
#define trfifoCreate() TRFifo_create()

// bool trfifoRegisterTask(TRFifo* self, ComplexTask* task);
//
// Registers a task with the shared resource. This enables the task to be advanced
// when the resource is available and reserves a slot for things like FIFO queues.
// This registration is consumed by a successful acquisition.
#define trfifoRegisterTask(self, task) (self)->_->registerTask(TRFifo(self), ComplexTask(task))
// bool trfifoCanAcquire(TRFifo* self, ComplexTask* task);
//
// Is it even possible for the given task to try to acquire the resource right now?
#define trfifoCanAcquire(self, task) (self)->_->canAcquire(TRFifo(self), ComplexTask(task))
// bool trfifoTryAcquire(TRFifo* self, ComplexTask* task);
//
// Try to acquire the resource. State tracking is up to the caller!
#define trfifoTryAcquire(self, task) (self)->_->tryAcquire(TRFifo(self), ComplexTask(task))
// void trfifoRelease(TRFifo* self, ComplexTask* task);
//
// Release the resource. State tracking is up to the caller!
#define trfifoRelease(self, task) (self)->_->release(TRFifo(self), ComplexTask(task))

