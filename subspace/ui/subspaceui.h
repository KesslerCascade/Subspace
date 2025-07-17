#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include "subspace.h"
#include "lang/lang.h"

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
typedef struct SettingsWin SettingsWin;
typedef struct SettingsWin_WeakRef SettingsWin_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct SubspaceUI SubspaceUI;
typedef struct SubspaceUI_WeakRef SubspaceUI_WeakRef;
typedef struct UINotifyDispatch UINotifyDispatch;
typedef struct UINotifyDispatch_WeakRef UINotifyDispatch_WeakRef;
saDeclarePtr(SubspaceUI);
saDeclarePtr(SubspaceUI_WeakRef);
saDeclarePtr(UINotifyDispatch);
saDeclarePtr(UINotifyDispatch_WeakRef);

#define ssuiNotify(self, event, ...) SubspaceUI__notify(self, event, count_macro_args(__VA_ARGS__), (stvar[]){ __VA_ARGS__ })

typedef struct UINotifyListener {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    void (*uiNotify)(_In_ void* self, _In_opt_ strref event, stvlist* params);
} UINotifyListener;
extern UINotifyListener UINotifyListener_tmpl;

typedef struct SubspaceUI_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*initialize)(_In_ void* self);
    bool (*shutdown)(_In_ void* self);
    void (*start)(_In_ void* self);
    void (*stop)(_In_ void* self);
    void (*update)(_In_ void* self);
    void (*updateMain)(_In_ void* self, _In_opt_ strref panel);
    void (*updateSettings)(_In_ void* self, _In_opt_ strref page);
    bool (*listen)(_In_ void* self, ObjInst* listener, _In_opt_ strref event);
} SubspaceUI_ClassIf;
extern SubspaceUI_ClassIf SubspaceUI_ClassIf_tmpl;

typedef struct UINotifyDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
    intptr (*cmp)(_In_ void* self, void* other, uint32 flags);
    uint32 (*hash)(_In_ void* self, uint32 flags);
} UINotifyDispatch_ClassIf;
extern UINotifyDispatch_ClassIf UINotifyDispatch_ClassIf_tmpl;

typedef struct SubspaceUI {
    union {
        SubspaceUI_ClassIf* _;
        void* _is_SubspaceUI;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    Subspace* ss;
    TaskQueue* uiq;
    bool started;
    MainWin* mainw;
    SettingsWin* settingsw;
    RWLock listenerlock;
    hashtable listeners;
} SubspaceUI;
extern ObjClassInfo SubspaceUI_clsinfo;
#define SubspaceUI(inst) ((SubspaceUI*)(unused_noeval((inst) && &((inst)->_is_SubspaceUI)), (inst)))
#define SubspaceUINone ((SubspaceUI*)NULL)

typedef struct SubspaceUI_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_SubspaceUI_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} SubspaceUI_WeakRef;
#define SubspaceUI_WeakRef(inst) ((SubspaceUI_WeakRef*)(unused_noeval((inst) && &((inst)->_is_SubspaceUI_WeakRef)), (inst)))

_objfactory_guaranteed SubspaceUI* SubspaceUI_create(Subspace* subspace);
// SubspaceUI* ssuiCreate(Subspace* subspace);
#define ssuiCreate(subspace) SubspaceUI_create(subspace)

void SubspaceUI__notify(_In_ SubspaceUI* self, _In_opt_ strref event, int n, stvar params[]);
// void ssui_notify(SubspaceUI* self, strref event, int n, stvar params[]);
#define ssui_notify(self, event, n, params) SubspaceUI__notify(SubspaceUI(self), event, n, params)

// bool ssuiInitialize(SubspaceUI* self);
#define ssuiInitialize(self) (self)->_->initialize(SubspaceUI(self))
// bool ssuiShutdown(SubspaceUI* self);
#define ssuiShutdown(self) (self)->_->shutdown(SubspaceUI(self))
// void ssuiStart(SubspaceUI* self);
#define ssuiStart(self) (self)->_->start(SubspaceUI(self))
// void ssuiStop(SubspaceUI* self);
#define ssuiStop(self) (self)->_->stop(SubspaceUI(self))
// void ssuiUpdate(SubspaceUI* self);
#define ssuiUpdate(self) (self)->_->update(SubspaceUI(self))
// void ssuiUpdateMain(SubspaceUI* self, strref panel);
#define ssuiUpdateMain(self, panel) (self)->_->updateMain(SubspaceUI(self), panel)
// void ssuiUpdateSettings(SubspaceUI* self, strref page);
#define ssuiUpdateSettings(self, page) (self)->_->updateSettings(SubspaceUI(self), page)
// bool ssuiListen(SubspaceUI* self, ObjInst* listener, strref event);
#define ssuiListen(self, listener, event) (self)->_->listen(SubspaceUI(self), ObjInst(listener), event)

typedef struct UINotifyDispatch {
    union {
        UINotifyDispatch_ClassIf* _;
        void* _is_UINotifyDispatch;
        void* _is_ComplexTask;
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
    int64 nextrun;        // next time for this task to run when scheduled
    int64 lastprogress;        // timestamp of last progress change
    Weak(ComplexTaskQueue)* lastq;        // The last queue this task ran on before it was deferred
    sa_TaskRequires _requires;        // list of requirements that must be satisfied
    uint16 flags;        // flags to customize task behavior
    uint16 _intflags;        // internal flags reserved for use by the scheduler
    atomic(uint32) _advcount;        // number of times this task has been advanced
    SubspaceUI* ui;
    string event;
    sa_stvar params;
} UINotifyDispatch;
extern ObjClassInfo UINotifyDispatch_clsinfo;
#define UINotifyDispatch(inst) ((UINotifyDispatch*)(unused_noeval((inst) && &((inst)->_is_UINotifyDispatch)), (inst)))
#define UINotifyDispatchNone ((UINotifyDispatch*)NULL)

typedef struct UINotifyDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_UINotifyDispatch_WeakRef;
        void* _is_ComplexTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} UINotifyDispatch_WeakRef;
#define UINotifyDispatch_WeakRef(inst) ((UINotifyDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_UINotifyDispatch_WeakRef)), (inst)))

_objfactory_guaranteed UINotifyDispatch* UINotifyDispatch_create(SubspaceUI* ui, _In_opt_ strref event, int n, stvar params[]);
// UINotifyDispatch* uinotifydispatchCreate(SubspaceUI* ui, strref event, int n, stvar params[]);
#define uinotifydispatchCreate(ui, event, n, params) UINotifyDispatch_create(SubspaceUI(ui), event, n, params)

// void uinotifydispatchRequireTask(UINotifyDispatch* self, Task* dep, bool failok);
//
// Wrapper around require() to depend on a task completing
#define uinotifydispatchRequireTask(self, dep, failok) ComplexTask_requireTask(ComplexTask(self), Task(dep), failok)

// void uinotifydispatchRequireTaskTimeout(UINotifyDispatch* self, Task* dep, bool failok, int64 timeout);
#define uinotifydispatchRequireTaskTimeout(self, dep, failok, timeout) ComplexTask_requireTaskTimeout(ComplexTask(self), Task(dep), failok, timeout)

// void uinotifydispatchRequireResource(UINotifyDispatch* self, TaskResource* res);
//
// Wrapper around require() to depend on acquiring a resource
#define uinotifydispatchRequireResource(self, res) ComplexTask_requireResource(ComplexTask(self), TaskResource(res))

// void uinotifydispatchRequireResourceTimeout(UINotifyDispatch* self, TaskResource* res, int64 timeout);
#define uinotifydispatchRequireResourceTimeout(self, res, timeout) ComplexTask_requireResourceTimeout(ComplexTask(self), TaskResource(res), timeout)

// void uinotifydispatchRequireGate(UINotifyDispatch* self, TRGate* gate);
//
// Wrapper around require() to depend on a gate being opened
#define uinotifydispatchRequireGate(self, gate) ComplexTask_requireGate(ComplexTask(self), TRGate(gate))

// void uinotifydispatchRequireGateTimeout(UINotifyDispatch* self, TRGate* gate, int64 timeout);
#define uinotifydispatchRequireGateTimeout(self, gate, timeout) ComplexTask_requireGateTimeout(ComplexTask(self), TRGate(gate), timeout)

// void uinotifydispatchRequire(UINotifyDispatch* self, TaskRequires* req);
//
// Add a requirement for the task to run
#define uinotifydispatchRequire(self, req) ComplexTask_require(ComplexTask(self), TaskRequires(req))

// bool uinotifydispatchAdvance(UINotifyDispatch* self);
//
// advance a deferred task to run as soon as possible
#define uinotifydispatchAdvance(self) ComplexTask_advance(ComplexTask(self))

// uint32 uinotifydispatchCheckRequires(UINotifyDispatch* self, bool updateProgress, int64* expires);
//
// check if this task can run because all requirements are satisfied
#define uinotifydispatchCheckRequires(self, updateProgress, expires) ComplexTask_checkRequires(ComplexTask(self), updateProgress, expires)

// void uinotifydispatchCancelRequires(UINotifyDispatch* self);
//
// cascade a task cancellation to any requirements
#define uinotifydispatchCancelRequires(self) ComplexTask_cancelRequires(ComplexTask(self))

// bool uinotifydispatchAcquireRequires(UINotifyDispatch* self, sa_TaskRequires* acquired);
//
// try to acquire required resources
#define uinotifydispatchAcquireRequires(self, acquired) ComplexTask_acquireRequires(ComplexTask(self), acquired)

// bool uinotifydispatchReleaseRequires(UINotifyDispatch* self, sa_TaskRequires resources);
//
// release a list of acquired resources
#define uinotifydispatchReleaseRequires(self, resources) ComplexTask_releaseRequires(ComplexTask(self), resources)

// bool uinotifydispatch_setState(UINotifyDispatch* self, uint32 newstate);
#define uinotifydispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 uinotifydispatchRun(UINotifyDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define uinotifydispatchRun(self, tq, worker, tcon) (self)->_->run(UINotifyDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void uinotifydispatchRunCancelled(UINotifyDispatch* self, TaskQueue* tq, TQWorker* worker);
#define uinotifydispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(UINotifyDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool uinotifydispatchCancel(UINotifyDispatch* self);
#define uinotifydispatchCancel(self) (self)->_->cancel(UINotifyDispatch(self))
// bool uinotifydispatchReset(UINotifyDispatch* self);
#define uinotifydispatchReset(self) (self)->_->reset(UINotifyDispatch(self))
// bool uinotifydispatchWait(UINotifyDispatch* self, int64 timeout);
#define uinotifydispatchWait(self, timeout) (self)->_->wait(UINotifyDispatch(self), timeout)
// intptr uinotifydispatchCmp(UINotifyDispatch* self, UINotifyDispatch* other, uint32 flags);
#define uinotifydispatchCmp(self, other, flags) (self)->_->cmp(UINotifyDispatch(self), other, flags)
// uint32 uinotifydispatchHash(UINotifyDispatch* self, uint32 flags);
#define uinotifydispatchHash(self, flags) (self)->_->hash(UINotifyDispatch(self), flags)

