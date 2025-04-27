#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include <iup.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct ComplexTask ComplexTask;
typedef struct ComplexTask_WeakRef ComplexTask_WeakRef;
typedef struct TRGate TRGate;
typedef struct TRGate_WeakRef TRGate_WeakRef;
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct IupRefreshDispatch IupRefreshDispatch;
typedef struct IupRefreshDispatch_WeakRef IupRefreshDispatch_WeakRef;
saDeclarePtr(IupRefreshDispatch);
saDeclarePtr(IupRefreshDispatch_WeakRef);

typedef struct IupRefreshDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} IupRefreshDispatch_ClassIf;
extern IupRefreshDispatch_ClassIf IupRefreshDispatch_ClassIf_tmpl;

typedef struct IupRefreshDispatch {
    union {
        IupRefreshDispatch_ClassIf* _;
        void* _is_IupRefreshDispatch;
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
    Ihandle* h;
} IupRefreshDispatch;
extern ObjClassInfo IupRefreshDispatch_clsinfo;
#define IupRefreshDispatch(inst) ((IupRefreshDispatch*)(unused_noeval((inst) && &((inst)->_is_IupRefreshDispatch)), (inst)))
#define IupRefreshDispatchNone ((IupRefreshDispatch*)NULL)

typedef struct IupRefreshDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_IupRefreshDispatch_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} IupRefreshDispatch_WeakRef;
#define IupRefreshDispatch_WeakRef(inst) ((IupRefreshDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_IupRefreshDispatch_WeakRef)), (inst)))

_objfactory_guaranteed IupRefreshDispatch* IupRefreshDispatch_create(Ihandle* hdl);
// IupRefreshDispatch* iuprefreshdispatchCreate(Ihandle* hdl);
#define iuprefreshdispatchCreate(hdl) IupRefreshDispatch_create(hdl)

// bool iuprefreshdispatch_setState(IupRefreshDispatch* self, uint32 newstate);
#define iuprefreshdispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 iuprefreshdispatchRun(IupRefreshDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define iuprefreshdispatchRun(self, tq, worker, tcon) (self)->_->run(IupRefreshDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void iuprefreshdispatchRunCancelled(IupRefreshDispatch* self, TaskQueue* tq, TQWorker* worker);
#define iuprefreshdispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(IupRefreshDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool iuprefreshdispatchCancel(IupRefreshDispatch* self);
#define iuprefreshdispatchCancel(self) (self)->_->cancel(IupRefreshDispatch(self))
// bool iuprefreshdispatchReset(IupRefreshDispatch* self);
#define iuprefreshdispatchReset(self) (self)->_->reset(IupRefreshDispatch(self))
// bool iuprefreshdispatchWait(IupRefreshDispatch* self, int64 timeout);
#define iuprefreshdispatchWait(self, timeout) (self)->_->wait(IupRefreshDispatch(self), timeout)

