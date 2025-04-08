#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue.h>
#include <lua.h>

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
typedef struct LuaTask LuaTask;
typedef struct LuaTask_WeakRef LuaTask_WeakRef;
saDeclarePtr(LuaTask);
saDeclarePtr(LuaTask_WeakRef);

bool luaPushTask(lua_State *L, Task *task);
bool luaParseTaskParams(LuaThread *lt, Task *task, int nparam);

bool registerTaskMetatable(lua_State *L);

typedef struct LuaTask_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaTask_ClassIf;
extern LuaTask_ClassIf LuaTask_ClassIf_tmpl;

typedef struct LuaTask {
    union {
        LuaTask_ClassIf* _;
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
} LuaTask;
extern ObjClassInfo LuaTask_clsinfo;
#define LuaTask(inst) ((LuaTask*)(unused_noeval((inst) && &((inst)->_is_LuaTask)), (inst)))
#define LuaTaskNone ((LuaTask*)NULL)

typedef struct LuaTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaTask_WeakRef;
#define LuaTask_WeakRef(inst) ((LuaTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaTask_WeakRef)), (inst)))

// bool luatask_setState(LuaTask* self, uint32 newstate);
#define luatask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luataskRun(LuaTask* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luataskRun(self, tq, worker, tcon) (self)->_->run(LuaTask(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luataskRunCancelled(LuaTask* self, TaskQueue* tq, TQWorker* worker);
#define luataskRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaTask(self), TaskQueue(tq), TQWorker(worker))
// bool luataskCancel(LuaTask* self);
#define luataskCancel(self) (self)->_->cancel(LuaTask(self))
// bool luataskReset(LuaTask* self);
#define luataskReset(self) (self)->_->reset(LuaTask(self))
// bool luataskWait(LuaTask* self, int64 timeout);
#define luataskWait(self, timeout) (self)->_->wait(LuaTask(self), timeout)

