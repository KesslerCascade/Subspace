#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "luatask.h"
#include "lua/luathread.h"
#include <cx/fs.h>
#include <cx/stype/stvar.h>

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
typedef struct LuaThread LuaThread;
typedef struct LuaCall LuaCall;
typedef struct LuaCall_WeakRef LuaCall_WeakRef;
typedef struct LuaCallbackDispatch LuaCallbackDispatch;
typedef struct LuaCallbackDispatch_WeakRef LuaCallbackDispatch_WeakRef;
typedef struct LuaFuncRef LuaFuncRef;
typedef struct LuaFuncRef_WeakRef LuaFuncRef_WeakRef;
typedef struct LuaFuncRefCleanup LuaFuncRefCleanup;
typedef struct LuaFuncRefCleanup_WeakRef LuaFuncRefCleanup_WeakRef;
saDeclarePtr(LuaCall);
saDeclarePtr(LuaCall_WeakRef);
saDeclarePtr(LuaCallbackDispatch);
saDeclarePtr(LuaCallbackDispatch_WeakRef);
saDeclarePtr(LuaFuncRef);
saDeclarePtr(LuaFuncRef_WeakRef);
saDeclarePtr(LuaFuncRefCleanup);
saDeclarePtr(LuaFuncRefCleanup_WeakRef);

#define luacallCreate(luastate, funcname, mustexist, maxresults, ...) luacall_create(luastate, funcname, mustexist, maxresults, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })
bool luaCallback(stvlist *cvars, stvlist *args);

typedef struct LuaCall_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaCall_ClassIf;
extern LuaCall_ClassIf LuaCall_ClassIf_tmpl;

typedef struct LuaCallbackDispatch_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaCallbackDispatch_ClassIf;
extern LuaCallbackDispatch_ClassIf LuaCallbackDispatch_ClassIf_tmpl;

typedef struct LuaFuncRefCleanup_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaFuncRefCleanup_ClassIf;
extern LuaFuncRefCleanup_ClassIf LuaFuncRefCleanup_ClassIf_tmpl;

typedef struct LuaCall {
    union {
        LuaCall_ClassIf* _;
        void* _is_LuaCall;
        void* _is_LuaTask;
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
    LuaThread* lt;
    string funcname;
    bool mustexist;
    int maxresults;
    sa_stvar args;
    sa_stvar results;
} LuaCall;
extern ObjClassInfo LuaCall_clsinfo;
#define LuaCall(inst) ((LuaCall*)(unused_noeval((inst) && &((inst)->_is_LuaCall)), (inst)))
#define LuaCallNone ((LuaCall*)NULL)

typedef struct LuaCall_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaCall_WeakRef;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaCall_WeakRef;
#define LuaCall_WeakRef(inst) ((LuaCall_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaCall_WeakRef)), (inst)))

_objfactory_guaranteed LuaCall* LuaCall__create(LuaThread* lt, _In_opt_ strref funcname, bool mustexist, int maxresults, int nargs, stvar args[]);
// LuaCall* luacall_create(LuaThread* lt, strref funcname, bool mustexist, int maxresults, int nargs, stvar args[]);
#define luacall_create(lt, funcname, mustexist, maxresults, nargs, args) LuaCall__create(lt, funcname, mustexist, maxresults, nargs, args)

int LuaCall_pushargs(lua_State* L, sa_stvar args);
// int luacallPushargs(lua_State* L, sa_stvar args);
#define luacallPushargs(L, args) LuaCall_pushargs(L, args)

int LuaCall_popresults(lua_State* L, int maxresults, sa_stvar* results);
// int luacallPopresults(lua_State* L, int maxresults, sa_stvar* results);
#define luacallPopresults(L, maxresults, results) LuaCall_popresults(L, maxresults, results)

bool LuaCall_exec(lua_State* L, _In_opt_ strref funcname, bool mustexist, sa_stvar args, int maxresults, sa_stvar* results);
// bool luacallExec(lua_State* L, strref funcname, bool mustexist, sa_stvar args, int maxresults, sa_stvar* results);
#define luacallExec(L, funcname, mustexist, args, maxresults, results) LuaCall_exec(L, funcname, mustexist, args, maxresults, results)

// bool luacall_setState(LuaCall* self, uint32 newstate);
#define luacall_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luacallRun(LuaCall* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luacallRun(self, tq, worker, tcon) (self)->_->run(LuaCall(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luacallRunCancelled(LuaCall* self, TaskQueue* tq, TQWorker* worker);
#define luacallRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaCall(self), TaskQueue(tq), TQWorker(worker))
// bool luacallCancel(LuaCall* self);
#define luacallCancel(self) (self)->_->cancel(LuaCall(self))
// bool luacallReset(LuaCall* self);
#define luacallReset(self) (self)->_->reset(LuaCall(self))
// bool luacallWait(LuaCall* self, int64 timeout);
#define luacallWait(self, timeout) (self)->_->wait(LuaCall(self), timeout)

typedef struct LuaCallbackDispatch {
    union {
        LuaCallbackDispatch_ClassIf* _;
        void* _is_LuaCallbackDispatch;
        void* _is_LuaCall;
        void* _is_LuaTask;
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
    LuaThread* lt;
    string funcname;
    bool mustexist;
    int maxresults;
    sa_stvar args;
    sa_stvar results;
    LuaFuncRef* func;
} LuaCallbackDispatch;
extern ObjClassInfo LuaCallbackDispatch_clsinfo;
#define LuaCallbackDispatch(inst) ((LuaCallbackDispatch*)(unused_noeval((inst) && &((inst)->_is_LuaCallbackDispatch)), (inst)))
#define LuaCallbackDispatchNone ((LuaCallbackDispatch*)NULL)

typedef struct LuaCallbackDispatch_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaCallbackDispatch_WeakRef;
        void* _is_LuaCall_WeakRef;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaCallbackDispatch_WeakRef;
#define LuaCallbackDispatch_WeakRef(inst) ((LuaCallbackDispatch_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaCallbackDispatch_WeakRef)), (inst)))

_objfactory_guaranteed LuaCallbackDispatch* LuaCallbackDispatch_create(LuaThread* lt, LuaFuncRef* funcref, int maxresults, int nargs, stvar args[]);
// LuaCallbackDispatch* luacallbackdispatchCreate(LuaThread* lt, LuaFuncRef* funcref, int maxresults, int nargs, stvar args[]);
#define luacallbackdispatchCreate(lt, funcref, maxresults, nargs, args) LuaCallbackDispatch_create(lt, LuaFuncRef(funcref), maxresults, nargs, args)

bool LuaCallbackDispatch_exec(lua_State* L, int ref, sa_stvar args, int maxresults, sa_stvar* results);
// bool luacallbackdispatchExec(lua_State* L, int ref, sa_stvar args, int maxresults, sa_stvar* results);
#define luacallbackdispatchExec(L, ref, args, maxresults, results) LuaCallbackDispatch_exec(L, ref, args, maxresults, results)

// bool luacallbackdispatch_setState(LuaCallbackDispatch* self, uint32 newstate);
#define luacallbackdispatch_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luacallbackdispatchRun(LuaCallbackDispatch* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luacallbackdispatchRun(self, tq, worker, tcon) (self)->_->run(LuaCallbackDispatch(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luacallbackdispatchRunCancelled(LuaCallbackDispatch* self, TaskQueue* tq, TQWorker* worker);
#define luacallbackdispatchRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaCallbackDispatch(self), TaskQueue(tq), TQWorker(worker))
// bool luacallbackdispatchCancel(LuaCallbackDispatch* self);
#define luacallbackdispatchCancel(self) (self)->_->cancel(LuaCallbackDispatch(self))
// bool luacallbackdispatchReset(LuaCallbackDispatch* self);
#define luacallbackdispatchReset(self) (self)->_->reset(LuaCallbackDispatch(self))
// bool luacallbackdispatchWait(LuaCallbackDispatch* self, int64 timeout);
#define luacallbackdispatchWait(self, timeout) (self)->_->wait(LuaCallbackDispatch(self), timeout)

typedef struct LuaFuncRef {
    union {
        ObjIface* _;
        void* _is_LuaFuncRef;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    LuaThread* lt;
    int ref;
} LuaFuncRef;
extern ObjClassInfo LuaFuncRef_clsinfo;
#define LuaFuncRef(inst) ((LuaFuncRef*)(unused_noeval((inst) && &((inst)->_is_LuaFuncRef)), (inst)))
#define LuaFuncRefNone ((LuaFuncRef*)NULL)

typedef struct LuaFuncRef_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaFuncRef_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaFuncRef_WeakRef;
#define LuaFuncRef_WeakRef(inst) ((LuaFuncRef_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaFuncRef_WeakRef)), (inst)))

_objfactory_check LuaFuncRef* LuaFuncRef_create(LuaThread* lt);
// LuaFuncRef* luafuncrefCreate(LuaThread* lt);
#define luafuncrefCreate(lt) LuaFuncRef_create(lt)


typedef struct LuaFuncRefCleanup {
    union {
        LuaFuncRefCleanup_ClassIf* _;
        void* _is_LuaFuncRefCleanup;
        void* _is_LuaTask;
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
    LuaThread* lt;
    int ref;
} LuaFuncRefCleanup;
extern ObjClassInfo LuaFuncRefCleanup_clsinfo;
#define LuaFuncRefCleanup(inst) ((LuaFuncRefCleanup*)(unused_noeval((inst) && &((inst)->_is_LuaFuncRefCleanup)), (inst)))
#define LuaFuncRefCleanupNone ((LuaFuncRefCleanup*)NULL)

typedef struct LuaFuncRefCleanup_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaFuncRefCleanup_WeakRef;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaFuncRefCleanup_WeakRef;
#define LuaFuncRefCleanup_WeakRef(inst) ((LuaFuncRefCleanup_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaFuncRefCleanup_WeakRef)), (inst)))

_objfactory_guaranteed LuaFuncRefCleanup* LuaFuncRefCleanup_create(LuaThread* lt, int ref);
// LuaFuncRefCleanup* luafuncrefcleanupCreate(LuaThread* lt, int ref);
#define luafuncrefcleanupCreate(lt, ref) LuaFuncRefCleanup_create(lt, ref)

// bool luafuncrefcleanup_setState(LuaFuncRefCleanup* self, uint32 newstate);
#define luafuncrefcleanup_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luafuncrefcleanupRun(LuaFuncRefCleanup* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luafuncrefcleanupRun(self, tq, worker, tcon) (self)->_->run(LuaFuncRefCleanup(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luafuncrefcleanupRunCancelled(LuaFuncRefCleanup* self, TaskQueue* tq, TQWorker* worker);
#define luafuncrefcleanupRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaFuncRefCleanup(self), TaskQueue(tq), TQWorker(worker))
// bool luafuncrefcleanupCancel(LuaFuncRefCleanup* self);
#define luafuncrefcleanupCancel(self) (self)->_->cancel(LuaFuncRefCleanup(self))
// bool luafuncrefcleanupReset(LuaFuncRefCleanup* self);
#define luafuncrefcleanupReset(self) (self)->_->reset(LuaFuncRefCleanup(self))
// bool luafuncrefcleanupWait(LuaFuncRefCleanup* self, int64 timeout);
#define luafuncrefcleanupWait(self, timeout) (self)->_->wait(LuaFuncRefCleanup(self), timeout)

