#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "basictask.h"
#include <cx/closure.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct Task Task;
typedef struct Task_WeakRef Task_WeakRef;
saDeclarePtr(Task);
saDeclarePtr(Task_WeakRef);

typedef struct Task_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} Task_ClassIf;
extern Task_ClassIf Task_ClassIf_tmpl;

typedef struct Task {
    union {
        Task_ClassIf* _;
        void* _is_Task;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    string name;        // task name to be shown in monitor output
    int64 last;        // the last time this task was moved between queues and/or run
    cchain oncomplete;        // functions that are called when this task has completed
} Task;
extern ObjClassInfo Task_clsinfo;
#define Task(inst) ((Task*)(unused_noeval((inst) && &((inst)->_is_Task)), (inst)))
#define TaskNone ((Task*)NULL)

typedef struct Task_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} Task_WeakRef;
#define Task_WeakRef(inst) ((Task_WeakRef*)(unused_noeval((inst) && &((inst)->_is_Task_WeakRef)), (inst)))

// bool ftask_setState(Task* self, uint32 newstate);
#define ftask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 ftaskRun(Task* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define ftaskRun(self, tq, worker, tcon) (self)->_->run(Task(self), TaskQueue(tq), TQWorker(worker), tcon)
// void ftaskRunCancelled(Task* self, TaskQueue* tq, TQWorker* worker);
#define ftaskRunCancelled(self, tq, worker) (self)->_->runCancelled(Task(self), TaskQueue(tq), TQWorker(worker))
// bool ftaskCancel(Task* self);
#define ftaskCancel(self) (self)->_->cancel(Task(self))
// bool ftaskReset(Task* self);
#define ftaskReset(self) (self)->_->reset(Task(self))
// bool ftaskWait(Task* self, int64 timeout);
#define ftaskWait(self, timeout) (self)->_->wait(Task(self), timeout)

