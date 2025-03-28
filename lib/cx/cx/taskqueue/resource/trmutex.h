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
typedef struct TRMutex TRMutex;
typedef struct TRMutex_WeakRef TRMutex_WeakRef;
saDeclarePtr(TRMutex);
saDeclarePtr(TRMutex_WeakRef);

typedef struct TRMutex_ClassIf {
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
    // releases a task from the wait list
    void (*wakeup)(_In_ void* self);
} TRMutex_ClassIf;
extern TRMutex_ClassIf TRMutex_ClassIf_tmpl;

typedef struct TRMutex {
    union {
        TRMutex_ClassIf* _;
        void* _is_TRMutex;
        void* _is_TaskResource;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Mutex mtx;
    Mutex _wlmtx;
    hashtable _waitlist;
} TRMutex;
extern ObjClassInfo TRMutex_clsinfo;
#define TRMutex(inst) ((TRMutex*)(unused_noeval((inst) && &((inst)->_is_TRMutex)), (inst)))
#define TRMutexNone ((TRMutex*)NULL)

typedef struct TRMutex_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TRMutex_WeakRef;
        void* _is_TaskResource_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TRMutex_WeakRef;
#define TRMutex_WeakRef(inst) ((TRMutex_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TRMutex_WeakRef)), (inst)))

_objfactory_guaranteed TRMutex* TRMutex_create();
// TRMutex* trmutexCreate();
#define trmutexCreate() TRMutex_create()

// bool trmutexRegisterTask(TRMutex* self, ComplexTask* task);
//
// Registers a task with the shared resource. This enables the task to be advanced
// when the resource is available and reserves a slot for things like FIFO queues.
// This registration is consumed by a successful acquisition.
#define trmutexRegisterTask(self, task) (self)->_->registerTask(TRMutex(self), ComplexTask(task))
// bool trmutexCanAcquire(TRMutex* self, ComplexTask* task);
//
// Is it even possible for the given task to try to acquire the resource right now?
#define trmutexCanAcquire(self, task) (self)->_->canAcquire(TRMutex(self), ComplexTask(task))
// bool trmutexTryAcquire(TRMutex* self, ComplexTask* task);
//
// Try to acquire the resource. State tracking is up to the caller!
#define trmutexTryAcquire(self, task) (self)->_->tryAcquire(TRMutex(self), ComplexTask(task))
// void trmutexRelease(TRMutex* self, ComplexTask* task);
//
// Release the resource. State tracking is up to the caller!
#define trmutexRelease(self, task) (self)->_->release(TRMutex(self), ComplexTask(task))
// void trmutexWakeup(TRMutex* self);
//
// releases a task from the wait list
#define trmutexWakeup(self) (self)->_->wakeup(TRMutex(self))

