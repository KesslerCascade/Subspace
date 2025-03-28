#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "tqueue.h"
#include <cx/taskqueue/task/complextask.h>

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
typedef struct ComplexTaskQueue ComplexTaskQueue;
typedef struct ComplexTaskQueue_WeakRef ComplexTaskQueue_WeakRef;
saDeclarePtr(ComplexTaskQueue);
saDeclarePtr(ComplexTaskQueue_WeakRef);

typedef struct ComplexTaskQueue_ClassIf {
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
    // add a task scheduled to run a relative time in the future
    bool (*schedule)(_In_ void* self, _In_ ComplexTask* task, int64 delay);
    // add a task but defer it indefinitely
    bool (*defer)(_In_ void* self, _In_ ComplexTask* task);
    bool (*advance)(_In_ void* self, _In_ ComplexTask* task);
} ComplexTaskQueue_ClassIf;
extern ComplexTaskQueue_ClassIf ComplexTaskQueue_ClassIf_tmpl;

typedef struct ComplexTaskQueue {
    union {
        ComplexTaskQueue_ClassIf* _;
        void* _is_ComplexTaskQueue;
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
    PrQueue advanceq;        // tasks that are being advanced out of the defer and/or schedule lists
    sa_ComplexTask scheduled;        // tasks that are scheduled to run at a later time, sorted by time
    hashtable deferred;        // tasks that are deferred indefinitely, to be held until they are advanced
} ComplexTaskQueue;
extern ObjClassInfo ComplexTaskQueue_clsinfo;
#define ComplexTaskQueue(inst) ((ComplexTaskQueue*)(unused_noeval((inst) && &((inst)->_is_ComplexTaskQueue)), (inst)))
#define ComplexTaskQueueNone ((ComplexTaskQueue*)NULL)

typedef struct ComplexTaskQueue_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_ComplexTaskQueue_WeakRef;
        void* _is_TaskQueue_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} ComplexTaskQueue_WeakRef;
#define ComplexTaskQueue_WeakRef(inst) ((ComplexTaskQueue_WeakRef*)(unused_noeval((inst) && &((inst)->_is_ComplexTaskQueue_WeakRef)), (inst)))

_objfactory_guaranteed ComplexTaskQueue* ComplexTaskQueue_create(_In_opt_ strref name, uint32 flags, int64 gcinterval, _In_ TQRunner* runner, _In_ TQManager* manager, _In_opt_ TQMonitor* monitor);
// ComplexTaskQueue* ctaskqueueCreate(strref name, uint32 flags, int64 gcinterval, TQRunner* runner, TQManager* manager, TQMonitor* monitor);
#define ctaskqueueCreate(name, flags, gcinterval, runner, manager, monitor) ComplexTaskQueue_create(name, flags, gcinterval, TQRunner(runner), TQManager(manager), TQMonitor(monitor))

// bool ctaskqueueStart(ComplexTaskQueue* self);
//
// start the queue, begin running tasks
#define ctaskqueueStart(self) (self)->_->start(ComplexTaskQueue(self))
// bool ctaskqueueStop(ComplexTaskQueue* self, int64 timeout);
//
// stop the queue
#define ctaskqueueStop(self, timeout) (self)->_->stop(ComplexTaskQueue(self), timeout)
// bool ctaskqueueAdd(ComplexTaskQueue* self, BasicTask* btask);
//
// add a task to the queue to run immediately
#define ctaskqueueAdd(self, btask) (self)->_->add(ComplexTaskQueue(self), BasicTask(btask))
// int64 ctaskqueueTick(ComplexTaskQueue* self);
//
// run one more more tasks -- only valid in manual mode
#define ctaskqueueTick(self) (self)->_->tick(ComplexTaskQueue(self))
// bool ctaskqueue_processDone(ComplexTaskQueue* self);
//
// internal function for manager to tell the queue to process its doneq
#define ctaskqueue_processDone(self) (self)->_->_processDone(ComplexTaskQueue(self))
// int64 ctaskqueue_processExtra(ComplexTaskQueue* self, bool taskscompleted);
//
// internal function for any additional processing that the queue needs to do in the manager thread
#define ctaskqueue_processExtra(self, taskscompleted) (self)->_->_processExtra(ComplexTaskQueue(self), taskscompleted)
// bool ctaskqueue_queueMaint(ComplexTaskQueue* self);
//
// internal function that the manager should call to perform any queue maintenance
#define ctaskqueue_queueMaint(self) (self)->_->_queueMaint(ComplexTaskQueue(self))
// bool ctaskqueue_runTask(ComplexTaskQueue* self, BasicTask** pbtask, TQWorker* worker);
//
// internal function workers should call to actually run a task and process the results
#define ctaskqueue_runTask(self, pbtask, worker) (self)->_->_runTask(ComplexTaskQueue(self), pbtask, TQWorker(worker))
// void ctaskqueue_clear(ComplexTaskQueue* self);
//
// deletes all tasks in queue, for internal use only
#define ctaskqueue_clear(self) (self)->_->_clear(ComplexTaskQueue(self))
// bool ctaskqueueSchedule(ComplexTaskQueue* self, ComplexTask* task, int64 delay);
//
// add a task scheduled to run a relative time in the future
#define ctaskqueueSchedule(self, task, delay) (self)->_->schedule(ComplexTaskQueue(self), ComplexTask(task), delay)
// bool ctaskqueueDefer(ComplexTaskQueue* self, ComplexTask* task);
//
// add a task but defer it indefinitely
#define ctaskqueueDefer(self, task) (self)->_->defer(ComplexTaskQueue(self), ComplexTask(task))
// bool ctaskqueueAdvance(ComplexTaskQueue* self, ComplexTask* task);
#define ctaskqueueAdvance(self, task) (self)->_->advance(ComplexTaskQueue(self), ComplexTask(task))

