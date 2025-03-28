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
typedef struct TaskControl TaskControl;
typedef struct BasicTask BasicTask;
typedef struct BasicTask_WeakRef BasicTask_WeakRef;
saDeclarePtr(BasicTask);
saDeclarePtr(BasicTask_WeakRef);

enum BasicTaskRunResultEnum {
    TASK_Result_Failure,
    TASK_Result_Success,
    TASK_Result_Basic_Count,
};

typedef struct BasicTask_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
} BasicTask_ClassIf;
extern BasicTask_ClassIf BasicTask_ClassIf_tmpl;

typedef struct BasicTask {
    union {
        BasicTask_ClassIf* _;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
} BasicTask;
extern ObjClassInfo BasicTask_clsinfo;
#define BasicTask(inst) ((BasicTask*)(unused_noeval((inst) && &((inst)->_is_BasicTask)), (inst)))
#define BasicTaskNone ((BasicTask*)NULL)

typedef struct BasicTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} BasicTask_WeakRef;
#define BasicTask_WeakRef(inst) ((BasicTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_BasicTask_WeakRef)), (inst)))

bool BasicTask__setState(_In_ BasicTask* self, uint32 newstate);
// bool btask_setState(BasicTask* self, uint32 newstate);
#define btask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 btaskRun(BasicTask* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define btaskRun(self, tq, worker, tcon) (self)->_->run(BasicTask(self), TaskQueue(tq), TQWorker(worker), tcon)
// void btaskRunCancelled(BasicTask* self, TaskQueue* tq, TQWorker* worker);
#define btaskRunCancelled(self, tq, worker) (self)->_->runCancelled(BasicTask(self), TaskQueue(tq), TQWorker(worker))
// bool btaskCancel(BasicTask* self);
#define btaskCancel(self) (self)->_->cancel(BasicTask(self))
// bool btaskReset(BasicTask* self);
#define btaskReset(self) (self)->_->reset(BasicTask(self))

