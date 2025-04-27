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
typedef struct IupAttrDispatch IupAttrDispatch;
typedef struct IupAttrDispatch_WeakRef IupAttrDispatch_WeakRef;
saDeclarePtr(IupAttrDispatch);
saDeclarePtr(IupAttrDispatch_WeakRef);

typedef struct IupAttrDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} IupAttrDispatch_ClassIf;
extern IupAttrDispatch_ClassIf IupAttrDispatch_ClassIf_tmpl;

typedef struct IupAttrDispatch {
    union {
        IupAttrDispatch_ClassIf* _;
        void* _is_IupAttrDispatch;
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
    string attr;
    string val;
    Ihandle* vhdl;
} IupAttrDispatch;
extern ObjClassInfo IupAttrDispatch_clsinfo;
#define IupAttrDispatch(inst) ((IupAttrDispatch*)(unused_noeval((inst) && &((inst)->_is_IupAttrDispatch)), (inst)))
#define IupAttrDispatchNone ((IupAttrDispatch*)NULL)

typedef struct IupAttrDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_IupAttrDispatch_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} IupAttrDispatch_WeakRef;
#define IupAttrDispatch_WeakRef(inst) ((IupAttrDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_IupAttrDispatch_WeakRef)), (inst)))

_objfactory_guaranteed IupAttrDispatch* IupAttrDispatch_create(Ihandle* hdl, _In_opt_ strref attr, _In_opt_ strref val);
// IupAttrDispatch* iupattrdispatchCreate(Ihandle* hdl, strref attr, strref val);
#define iupattrdispatchCreate(hdl, attr, val) IupAttrDispatch_create(hdl, attr, val)

_objfactory_guaranteed IupAttrDispatch* IupAttrDispatch_createHandle(Ihandle* hdl, _In_opt_ strref attr, Ihandle* vhdl);
// IupAttrDispatch* iupattrdispatchCreateHandle(Ihandle* hdl, strref attr, Ihandle* vhdl);
#define iupattrdispatchCreateHandle(hdl, attr, vhdl) IupAttrDispatch_createHandle(hdl, attr, vhdl)

// bool iupattrdispatch_setState(IupAttrDispatch* self, uint32 newstate);
#define iupattrdispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 iupattrdispatchRun(IupAttrDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define iupattrdispatchRun(self, tq, worker, tcon) (self)->_->run(IupAttrDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void iupattrdispatchRunCancelled(IupAttrDispatch* self, TaskQueue* tq, TQWorker* worker);
#define iupattrdispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(IupAttrDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool iupattrdispatchCancel(IupAttrDispatch* self);
#define iupattrdispatchCancel(self) (self)->_->cancel(IupAttrDispatch(self))
// bool iupattrdispatchReset(IupAttrDispatch* self);
#define iupattrdispatchReset(self) (self)->_->reset(IupAttrDispatch(self))
// bool iupattrdispatchWait(IupAttrDispatch* self, int64 timeout);
#define iupattrdispatchWait(self, timeout) (self)->_->wait(IupAttrDispatch(self), timeout)

