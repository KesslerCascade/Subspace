#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include <iup.h>
#include <ui/image/image.h>

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
typedef struct IupImageAttachDispatch IupImageAttachDispatch;
typedef struct IupImageAttachDispatch_WeakRef IupImageAttachDispatch_WeakRef;
saDeclarePtr(IupImageAttachDispatch);
saDeclarePtr(IupImageAttachDispatch_WeakRef);

typedef struct IupImageAttachDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} IupImageAttachDispatch_ClassIf;
extern IupImageAttachDispatch_ClassIf IupImageAttachDispatch_ClassIf_tmpl;

typedef struct IupImageAttachDispatch {
    union {
        IupImageAttachDispatch_ClassIf* _;
        void* _is_IupImageAttachDispatch;
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
    Image* img;
} IupImageAttachDispatch;
extern ObjClassInfo IupImageAttachDispatch_clsinfo;
#define IupImageAttachDispatch(inst) ((IupImageAttachDispatch*)(unused_noeval((inst) && &((inst)->_is_IupImageAttachDispatch)), (inst)))
#define IupImageAttachDispatchNone ((IupImageAttachDispatch*)NULL)

typedef struct IupImageAttachDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_IupImageAttachDispatch_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} IupImageAttachDispatch_WeakRef;
#define IupImageAttachDispatch_WeakRef(inst) ((IupImageAttachDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_IupImageAttachDispatch_WeakRef)), (inst)))

_objfactory_guaranteed IupImageAttachDispatch* IupImageAttachDispatch_create(Ihandle* hdl, _In_opt_ strref attr, Image* img);
// IupImageAttachDispatch* iupimageattachdispatchCreate(Ihandle* hdl, strref attr, Image* img);
#define iupimageattachdispatchCreate(hdl, attr, img) IupImageAttachDispatch_create(hdl, attr, Image(img))

// bool iupimageattachdispatch_setState(IupImageAttachDispatch* self, uint32 newstate);
#define iupimageattachdispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 iupimageattachdispatchRun(IupImageAttachDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define iupimageattachdispatchRun(self, tq, worker, tcon) (self)->_->run(IupImageAttachDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void iupimageattachdispatchRunCancelled(IupImageAttachDispatch* self, TaskQueue* tq, TQWorker* worker);
#define iupimageattachdispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(IupImageAttachDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool iupimageattachdispatchCancel(IupImageAttachDispatch* self);
#define iupimageattachdispatchCancel(self) (self)->_->cancel(IupImageAttachDispatch(self))
// bool iupimageattachdispatchReset(IupImageAttachDispatch* self);
#define iupimageattachdispatchReset(self) (self)->_->reset(IupImageAttachDispatch(self))
// bool iupimageattachdispatchWait(IupImageAttachDispatch* self, int64 timeout);
#define iupimageattachdispatchWait(self, timeout) (self)->_->wait(IupImageAttachDispatch(self), timeout)

