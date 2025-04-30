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
typedef struct IupSetImageDispatch IupSetImageDispatch;
typedef struct IupSetImageDispatch_WeakRef IupSetImageDispatch_WeakRef;
saDeclarePtr(IupSetImageDispatch);
saDeclarePtr(IupSetImageDispatch_WeakRef);

typedef struct IupSetImageDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} IupSetImageDispatch_ClassIf;
extern IupSetImageDispatch_ClassIf IupSetImageDispatch_ClassIf_tmpl;

typedef struct IupSetImageDispatch {
    union {
        IupSetImageDispatch_ClassIf* _;
        void* _is_IupSetImageDispatch;
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
    string iupname;
    Image* img;
} IupSetImageDispatch;
extern ObjClassInfo IupSetImageDispatch_clsinfo;
#define IupSetImageDispatch(inst) ((IupSetImageDispatch*)(unused_noeval((inst) && &((inst)->_is_IupSetImageDispatch)), (inst)))
#define IupSetImageDispatchNone ((IupSetImageDispatch*)NULL)

typedef struct IupSetImageDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_IupSetImageDispatch_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} IupSetImageDispatch_WeakRef;
#define IupSetImageDispatch_WeakRef(inst) ((IupSetImageDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_IupSetImageDispatch_WeakRef)), (inst)))

_objfactory_guaranteed IupSetImageDispatch* IupSetImageDispatch_create(_In_opt_ strref iupname, Image* img);
// IupSetImageDispatch* iupsetimagedispatchCreate(strref iupname, Image* img);
#define iupsetimagedispatchCreate(iupname, img) IupSetImageDispatch_create(iupname, Image(img))

// bool iupsetimagedispatch_setState(IupSetImageDispatch* self, uint32 newstate);
#define iupsetimagedispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 iupsetimagedispatchRun(IupSetImageDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define iupsetimagedispatchRun(self, tq, worker, tcon) (self)->_->run(IupSetImageDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void iupsetimagedispatchRunCancelled(IupSetImageDispatch* self, TaskQueue* tq, TQWorker* worker);
#define iupsetimagedispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(IupSetImageDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool iupsetimagedispatchCancel(IupSetImageDispatch* self);
#define iupsetimagedispatchCancel(self) (self)->_->cancel(IupSetImageDispatch(self))
// bool iupsetimagedispatchReset(IupSetImageDispatch* self);
#define iupsetimagedispatchReset(self) (self)->_->reset(IupSetImageDispatch(self))
// bool iupsetimagedispatchWait(IupSetImageDispatch* self, int64 timeout);
#define iupsetimagedispatchWait(self, timeout) (self)->_->wait(IupSetImageDispatch(self), timeout)

