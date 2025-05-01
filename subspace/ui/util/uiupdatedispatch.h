#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "ui/subspaceui.h"

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
typedef struct MainWin MainWin;
typedef struct MainWin_WeakRef MainWin_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct UIUpdateDispatch UIUpdateDispatch;
typedef struct UIUpdateDispatch_WeakRef UIUpdateDispatch_WeakRef;
saDeclarePtr(UIUpdateDispatch);
saDeclarePtr(UIUpdateDispatch_WeakRef);

typedef struct UIUpdateDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} UIUpdateDispatch_ClassIf;
extern UIUpdateDispatch_ClassIf UIUpdateDispatch_ClassIf_tmpl;

typedef struct UIUpdateDispatch {
    union {
        UIUpdateDispatch_ClassIf* _;
        void* _is_UIUpdateDispatch;
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
    SubspaceUI* ui;
    string panelname;
} UIUpdateDispatch;
extern ObjClassInfo UIUpdateDispatch_clsinfo;
#define UIUpdateDispatch(inst) ((UIUpdateDispatch*)(unused_noeval((inst) && &((inst)->_is_UIUpdateDispatch)), (inst)))
#define UIUpdateDispatchNone ((UIUpdateDispatch*)NULL)

typedef struct UIUpdateDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_UIUpdateDispatch_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} UIUpdateDispatch_WeakRef;
#define UIUpdateDispatch_WeakRef(inst) ((UIUpdateDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_UIUpdateDispatch_WeakRef)), (inst)))

_objfactory_guaranteed UIUpdateDispatch* UIUpdateDispatch_create(SubspaceUI* ui, _In_opt_ strref panelname);
// UIUpdateDispatch* uiupdatedispatchCreate(SubspaceUI* ui, strref panelname);
#define uiupdatedispatchCreate(ui, panelname) UIUpdateDispatch_create(SubspaceUI(ui), panelname)

// bool uiupdatedispatch_setState(UIUpdateDispatch* self, uint32 newstate);
#define uiupdatedispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 uiupdatedispatchRun(UIUpdateDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define uiupdatedispatchRun(self, tq, worker, tcon) (self)->_->run(UIUpdateDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void uiupdatedispatchRunCancelled(UIUpdateDispatch* self, TaskQueue* tq, TQWorker* worker);
#define uiupdatedispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(UIUpdateDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool uiupdatedispatchCancel(UIUpdateDispatch* self);
#define uiupdatedispatchCancel(self) (self)->_->cancel(UIUpdateDispatch(self))
// bool uiupdatedispatchReset(UIUpdateDispatch* self);
#define uiupdatedispatchReset(self) (self)->_->reset(UIUpdateDispatch(self))
// bool uiupdatedispatchWait(UIUpdateDispatch* self, int64 timeout);
#define uiupdatedispatchWait(self, timeout) (self)->_->wait(UIUpdateDispatch(self), timeout)

