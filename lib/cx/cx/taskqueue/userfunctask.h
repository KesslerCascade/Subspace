#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/task/basictask.h>
#include <cx/taskqueue/taskqueue.h>

typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct TaskControl TaskControl;
typedef struct UserFuncTask UserFuncTask;
typedef struct UserFuncTask_WeakRef UserFuncTask_WeakRef;
saDeclarePtr(UserFuncTask);
saDeclarePtr(UserFuncTask_WeakRef);

typedef struct UserFuncTask_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    uint32 (*run)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker, _Inout_ TaskControl* tcon);
    void (*runCancelled)(_In_ void* self, _In_ TaskQueue* tq, _In_ TQWorker* worker);
    bool (*cancel)(_In_ void* self);
    bool (*reset)(_In_ void* self);
} UserFuncTask_ClassIf;
extern UserFuncTask_ClassIf UserFuncTask_ClassIf_tmpl;

typedef struct UserFuncTask {
    union {
        UserFuncTask_ClassIf* _;
        void* _is_UserFuncTask;
        void* _is_BasicTask;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    atomic(uint32) state;
    UserTaskCB func;
    void* udata;
} UserFuncTask;
extern ObjClassInfo UserFuncTask_clsinfo;
#define UserFuncTask(inst) ((UserFuncTask*)(unused_noeval((inst) && &((inst)->_is_UserFuncTask)), (inst)))
#define UserFuncTaskNone ((UserFuncTask*)NULL)

typedef struct UserFuncTask_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_UserFuncTask_WeakRef;
        void* _is_BasicTask_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} UserFuncTask_WeakRef;
#define UserFuncTask_WeakRef(inst) ((UserFuncTask_WeakRef*)(unused_noeval((inst) && &((inst)->_is_UserFuncTask_WeakRef)), (inst)))

_objfactory_guaranteed UserFuncTask* UserFuncTask_create(UserTaskCB func, void* udata);
// UserFuncTask* userfunctaskCreate(UserTaskCB func, void* udata);
#define userfunctaskCreate(func, udata) UserFuncTask_create(func, udata)

// bool userfunctask_setState(UserFuncTask* self, uint32 newstate);
#define userfunctask_setState(self, newstate) BasicTask__setState(BasicTask(self), newstate)

// uint32 userfunctaskRun(UserFuncTask* self, TaskQueue* tq, TQWorker* worker, TaskControl* tcon);
#define userfunctaskRun(self, tq, worker, tcon) (self)->_->run(UserFuncTask(self), TaskQueue(tq), TQWorker(worker), tcon)
// void userfunctaskRunCancelled(UserFuncTask* self, TaskQueue* tq, TQWorker* worker);
#define userfunctaskRunCancelled(self, tq, worker) (self)->_->runCancelled(UserFuncTask(self), TaskQueue(tq), TQWorker(worker))
// bool userfunctaskCancel(UserFuncTask* self);
#define userfunctaskCancel(self) (self)->_->cancel(UserFuncTask(self))
// bool userfunctaskReset(UserFuncTask* self);
#define userfunctaskReset(self) (self)->_->reset(UserFuncTask(self))

