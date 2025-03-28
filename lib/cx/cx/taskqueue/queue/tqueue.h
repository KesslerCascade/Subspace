#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include <cx/taskqueue/taskqueue_shared.h>
#include <cx/thread/event.h>
#include <cx/thread/thread.h>
#include <cx/thread/prqueue.h>

typedef struct TQRunner TQRunner;
typedef struct TQRunner_WeakRef TQRunner_WeakRef;
typedef struct TQManager TQManager;
typedef struct TQManager_WeakRef TQManager_WeakRef;
typedef struct TQMonitor TQMonitor;
typedef struct TQMonitor_WeakRef TQMonitor_WeakRef;
typedef struct TQWorker TQWorker;
typedef struct TQWorker_WeakRef TQWorker_WeakRef;
typedef struct BasicTask BasicTask;
typedef struct BasicTask_WeakRef BasicTask_WeakRef;
typedef struct TaskQueue TaskQueue;
typedef struct TaskQueue_WeakRef TaskQueue_WeakRef;
saDeclarePtr(TaskQueue);
saDeclarePtr(TaskQueue_WeakRef);

typedef enum TaskQueueStateEnum {
    TQState_Init,
    TQState_Starting,
    TQState_Running,
    TQState_Stopping,
    TQState_Shutdown,
} TaskQueueStateEnum;

typedef struct TaskQueue_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    // start the queue, begin running tasks
    bool (*start)(_In_ void* self);
    // stop the queue
    bool (*stop)(_In_ void* self, int64 timeout);
    // add a task to the queue to run immediately
    bool (*add)(_In_ void* self, _In_ BasicTask* btask);
    // run one more more tasks -- only valid in manual mode
    int64 (*tick)(_In_ void* self);
    // internal function for manager to tell the queue to process its doneq
    bool (*_processDone)(_In_ void* self);
    // internal function for any additional processing that the queue needs to do in the manager thread
    int64 (*_processExtra)(_In_ void* self, bool taskscompleted);
    // internal function that the manager should call to perform any queue maintenance
    bool (*_queueMaint)(_In_ void* self);
    // internal function workers should call to actually run a task and process the results
    bool (*_runTask)(_In_ void* self, _Inout_ BasicTask** pbtask, _In_ TQWorker* worker);
    // deletes all tasks in queue, for internal use only
    void (*_clear)(_In_ void* self);
} TaskQueue_ClassIf;
extern TaskQueue_ClassIf TaskQueue_ClassIf_tmpl;

typedef struct TaskQueue {
    union {
        TaskQueue_ClassIf* _;
        void* _is_TaskQueue;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    string name;
    atomic(uint32) state;
    TQRunner* runner;
    TQManager* manager;
    TQMonitor* monitor;
    Event workev;        // signaled when there is work to be done
    PrQueue runq;        // tasks that are ready to be picked up by workers
    PrQueue doneq;        // tasks that are either deferred or finished
    int64 gcinterval;        // how often to run a garbage collection cycle on a queue
    uint32 flags;
    int64 _lastgc;        // timestamp of last GC cycle
    int _gccycle;        // which GC cycle ran last
} TaskQueue;
extern ObjClassInfo TaskQueue_clsinfo;
#define TaskQueue(inst) ((TaskQueue*)(unused_noeval((inst) && &((inst)->_is_TaskQueue)), (inst)))
#define TaskQueueNone ((TaskQueue*)NULL)

typedef struct TaskQueue_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_TaskQueue_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} TaskQueue_WeakRef;
#define TaskQueue_WeakRef(inst) ((TaskQueue_WeakRef*)(unused_noeval((inst) && &((inst)->_is_TaskQueue_WeakRef)), (inst)))

_objfactory_guaranteed TaskQueue* TaskQueue_create(_In_opt_ strref name, uint32 flags, int64 gcinterval, _In_ TQRunner* runner, _In_ TQManager* manager, _In_opt_ TQMonitor* monitor);
// TaskQueue* taskqueueCreate(strref name, uint32 flags, int64 gcinterval, TQRunner* runner, TQManager* manager, TQMonitor* monitor);
#define taskqueueCreate(name, flags, gcinterval, runner, manager, monitor) TaskQueue_create(name, flags, gcinterval, TQRunner(runner), TQManager(manager), TQMonitor(monitor))

// bool taskqueueStart(TaskQueue* self);
//
// start the queue, begin running tasks
#define taskqueueStart(self) (self)->_->start(TaskQueue(self))
// bool taskqueueStop(TaskQueue* self, int64 timeout);
//
// stop the queue
#define taskqueueStop(self, timeout) (self)->_->stop(TaskQueue(self), timeout)
// bool taskqueueAdd(TaskQueue* self, BasicTask* btask);
//
// add a task to the queue to run immediately
#define taskqueueAdd(self, btask) (self)->_->add(TaskQueue(self), BasicTask(btask))
// int64 taskqueueTick(TaskQueue* self);
//
// run one more more tasks -- only valid in manual mode
#define taskqueueTick(self) (self)->_->tick(TaskQueue(self))
// bool taskqueue_processDone(TaskQueue* self);
//
// internal function for manager to tell the queue to process its doneq
#define taskqueue_processDone(self) (self)->_->_processDone(TaskQueue(self))
// int64 taskqueue_processExtra(TaskQueue* self, bool taskscompleted);
//
// internal function for any additional processing that the queue needs to do in the manager thread
#define taskqueue_processExtra(self, taskscompleted) (self)->_->_processExtra(TaskQueue(self), taskscompleted)
// bool taskqueue_queueMaint(TaskQueue* self);
//
// internal function that the manager should call to perform any queue maintenance
#define taskqueue_queueMaint(self) (self)->_->_queueMaint(TaskQueue(self))
// bool taskqueue_runTask(TaskQueue* self, BasicTask** pbtask, TQWorker* worker);
//
// internal function workers should call to actually run a task and process the results
#define taskqueue_runTask(self, pbtask, worker) (self)->_->_runTask(TaskQueue(self), pbtask, TQWorker(worker))
// void taskqueue_clear(TaskQueue* self);
//
// deletes all tasks in queue, for internal use only
#define taskqueue_clear(self) (self)->_->_clear(TaskQueue(self))

