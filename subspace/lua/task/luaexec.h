#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "luatask.h"
#include "lua/luathread.h"
#include <cx/fs.h>

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
typedef struct LuaExecString LuaExecString;
typedef struct LuaExecString_WeakRef LuaExecString_WeakRef;
typedef struct LuaExecFile LuaExecFile;
typedef struct LuaExecFile_WeakRef LuaExecFile_WeakRef;
saDeclarePtr(LuaExecString);
saDeclarePtr(LuaExecString_WeakRef);
saDeclarePtr(LuaExecFile);
saDeclarePtr(LuaExecFile_WeakRef);

typedef struct LuaExecString_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaExecString_ClassIf;
extern LuaExecString_ClassIf LuaExecString_ClassIf_tmpl;

typedef struct LuaExecFile_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
    bool (*wait)(_In_ void* self, int64 timeout);
} LuaExecFile_ClassIf;
extern LuaExecFile_ClassIf LuaExecFile_ClassIf_tmpl;

typedef struct LuaExecString {
    union {
        LuaExecString_ClassIf* _;
        void* _is_LuaExecString;
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
    string scriptdata;
} LuaExecString;
extern ObjClassInfo LuaExecString_clsinfo;
#define LuaExecString(inst) ((LuaExecString*)(unused_noeval((inst) && &((inst)->_is_LuaExecString)), (inst)))
#define LuaExecStringNone ((LuaExecString*)NULL)

typedef struct LuaExecString_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaExecString_WeakRef;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaExecString_WeakRef;
#define LuaExecString_WeakRef(inst) ((LuaExecString_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaExecString_WeakRef)), (inst)))

_objfactory_guaranteed LuaExecString* LuaExecString_create(LuaThread* lt, _In_opt_ strref scriptdata);
// LuaExecString* luaexecstringCreate(LuaThread* lt, strref scriptdata);
#define luaexecstringCreate(lt, scriptdata) LuaExecString_create(lt, scriptdata)

bool LuaExecString_load(lua_State* L, _In_opt_ strref scriptdata);
// bool luaexecstringLoad(lua_State* L, strref scriptdata);
#define luaexecstringLoad(L, scriptdata) LuaExecString_load(L, scriptdata)

bool LuaExecString_exec(lua_State* L, _In_opt_ strref scriptdata);
// bool luaexecstringExec(lua_State* L, strref scriptdata);
#define luaexecstringExec(L, scriptdata) LuaExecString_exec(L, scriptdata)

// bool luaexecstring_setState(LuaExecString* self, uint32 newstate);
#define luaexecstring_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luaexecstringRun(LuaExecString* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luaexecstringRun(self, tq, worker, tcon) (self)->_->run(LuaExecString(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luaexecstringRunCancelled(LuaExecString* self, TaskQueue* tq, TQWorker* worker);
#define luaexecstringRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaExecString(self), TaskQueue(tq), TQWorker(worker))
// bool luaexecstringCancel(LuaExecString* self);
#define luaexecstringCancel(self) (self)->_->cancel(LuaExecString(self))
// bool luaexecstringReset(LuaExecString* self);
#define luaexecstringReset(self) (self)->_->reset(LuaExecString(self))
// bool luaexecstringWait(LuaExecString* self, int64 timeout);
#define luaexecstringWait(self, timeout) (self)->_->wait(LuaExecString(self), timeout)

typedef struct LuaExecFile {
    union {
        LuaExecFile_ClassIf* _;
        void* _is_LuaExecFile;
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
    string filename;
} LuaExecFile;
extern ObjClassInfo LuaExecFile_clsinfo;
#define LuaExecFile(inst) ((LuaExecFile*)(unused_noeval((inst) && &((inst)->_is_LuaExecFile)), (inst)))
#define LuaExecFileNone ((LuaExecFile*)NULL)

typedef struct LuaExecFile_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_LuaExecFile_WeakRef;
        void* _is_LuaTask_WeakRef;
        void* _is_Task_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} LuaExecFile_WeakRef;
#define LuaExecFile_WeakRef(inst) ((LuaExecFile_WeakRef*)(unused_noeval((inst) && &((inst)->_is_LuaExecFile_WeakRef)), (inst)))

_objfactory_guaranteed LuaExecFile* LuaExecFile_create(LuaThread* lt, _In_opt_ strref filename);
// LuaExecFile* luaexecfileCreate(LuaThread* lt, strref filename);
#define luaexecfileCreate(lt, filename) LuaExecFile_create(lt, filename)

bool LuaExecFile_load(lua_State* L, VFS* vfs, _In_opt_ strref filename);
// bool luaexecfileLoad(lua_State* L, VFS* vfs, strref filename);
#define luaexecfileLoad(L, vfs, filename) LuaExecFile_load(L, vfs, filename)

bool LuaExecFile_exec(lua_State* L, VFS* vfs, _In_opt_ strref filename);
// bool luaexecfileExec(lua_State* L, VFS* vfs, strref filename);
#define luaexecfileExec(L, vfs, filename) LuaExecFile_exec(L, vfs, filename)

// bool luaexecfile_setState(LuaExecFile* self, uint32 newstate);
#define luaexecfile_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 luaexecfileRun(LuaExecFile* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define luaexecfileRun(self, tq, worker, tcon) (self)->_->run(LuaExecFile(self), TaskQueue(tq), TQWorker(worker), tcon)
// void luaexecfileRunCancelled(LuaExecFile* self, TaskQueue* tq, TQWorker* worker);
#define luaexecfileRunCancelled(self, tq, worker) (self)->_->runCancelled(LuaExecFile(self), TaskQueue(tq), TQWorker(worker))
// bool luaexecfileCancel(LuaExecFile* self);
#define luaexecfileCancel(self) (self)->_->cancel(LuaExecFile(self))
// bool luaexecfileReset(LuaExecFile* self);
#define luaexecfileReset(self) (self)->_->reset(LuaExecFile(self))
// bool luaexecfileWait(LuaExecFile* self, int64 timeout);
#define luaexecfileWait(self, timeout) (self)->_->wait(LuaExecFile(self), timeout)

