#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>

typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TaskResource TaskResource;
typedef struct TaskResource_WeakRef TaskResource_WeakRef;
saDeclarePtr(TaskResource);
saDeclarePtr(TaskResource_WeakRef);

typedef struct TaskResource_ClassIf {
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
} TaskResource_ClassIf;
extern TaskResource_ClassIf TaskResource_ClassIf_tmpl;

typedef struct TaskResource {
    union {
        TaskResource_ClassIf* _;
        void* _is_TaskResource;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

} TaskResource;
extern ObjClassInfo TaskResource_clsinfo;
#define TaskResource(inst) ((TaskResource*)(unused_noeval((inst) && &((inst)->_is_TaskResource)), (inst)))
#define TaskResourceNone ((TaskResource*)NULL)

typedef struct TaskResource_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskResource_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskResource_WeakRef;
#define TaskResource_WeakRef(inst) ((TaskResource_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskResource_WeakRef)), (inst)))

// bool taskresourceRegisterTask(TaskResource* self, ComplexTask* task);
//
// Registers a task with the shared resource. This enables the task to be advanced
// when the resource is available and reserves a slot for things like FIFO queues.
// This registration is consumed by a successful acquisition.
#define taskresourceRegisterTask(self, task) (self)->_->registerTask(TaskResource(self), ComplexTask(task))
// bool taskresourceCanAcquire(TaskResource* self, ComplexTask* task);
//
// Is it even possible for the given task to try to acquire the resource right now?
#define taskresourceCanAcquire(self, task) (self)->_->canAcquire(TaskResource(self), ComplexTask(task))
// bool taskresourceTryAcquire(TaskResource* self, ComplexTask* task);
//
// Try to acquire the resource. State tracking is up to the caller!
#define taskresourceTryAcquire(self, task) (self)->_->tryAcquire(TaskResource(self), ComplexTask(task))
// void taskresourceRelease(TaskResource* self, ComplexTask* task);
//
// Release the resource. State tracking is up to the caller!
#define taskresourceRelease(self, task) (self)->_->release(TaskResource(self), ComplexTask(task))

