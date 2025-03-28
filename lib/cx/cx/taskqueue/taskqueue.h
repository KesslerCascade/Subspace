#pragma once

#include <cx/cx.h>
#include <cx/log/log.h>
#include <cx/taskqueue/queue/tqcomplex.h>
#include <cx/taskqueue/queue/tqueue.h>
#include <cx/taskqueue/requires/taskrequiresgate.h>
#include <cx/taskqueue/resource/trfifo.h>
#include <cx/taskqueue/resource/trlifo.h>
#include <cx/taskqueue/resource/trmutex.h>
#include <cx/taskqueue/task/mptask.h>
#include <cx/taskqueue/worker/tqworker.h>

// Object-oriented task queue suitable for use in heavily multithreaded applications
// Initialize a queue config for a queue with a single worker (i.e. a UI thread)
void tqPresetSingle(_Out_ TaskQueueConfig *tqconfig);

// Initialize a queue config with minimal presets based on the number of physical cores only
void tqPresetMinimal(_Out_ TaskQueueConfig *tqconfig);

// Initialize a queue config with balanced presets based on the number of logical cores
void tqPresetBalanced(_Out_ TaskQueueConfig *tqconfig);

// Initialize a queue config with presets tuned for high performance under heavy usage
void tqPresetHeavy(_Out_ TaskQueueConfig* tqconfig);

// Initialize a queue config for a queue without a thread pool, that will be ticked manaully
void tqPresetManual(_Out_ TaskQueueConfig* tqconfig);

// Update an existing config to enable sensible defaults for queue monitoring
void tqEnableMonitor(_Inout_ TaskQueueConfig *tqconfig);

// Create a task queue but does not start it
_Ret_opt_valid_ _Check_return_
TaskQueue *tqCreate(_In_ strref name, _In_ TaskQueueConfig *tqconfig);

// Start or re-start a queue.
#define tqStart taskqueueStart

// Tick a manual queue
#define tqTick taskqueueTick

// Attempt to shut down a queue.
// If wait is nonzero, will wait for workers to exit.
// Returns true if the queue is completely shut down and all worker threads
// are finished.
#define tqShutdown(tq, wait) taskqueueStop(tq, wait)

#define tqRelease(tq) objRelease(tq)

// bool tqAdd(TaskQueue *tq, BasicTask *task);
// Add a task to the queue to run immediately.
#define tqAdd(tq, task) taskqueueAdd(tq, task)

// void tqRun(TaskQueue *tq, BasicTask **ptask);
// Convenience function to run a task and release it.
#define tqRun(tq, ptask) do { taskqueueAdd(tq, *ptask); objRelease(ptask); } while(0)

_meta_inline bool _tqSchedule(_Inout_ TaskQueue* tq, _In_ ComplexTask* task, int64 delay)
{
    ComplexTaskQueue* ctq = objDynCast(ComplexTaskQueue, tq);
    if (!ctq)
        return false;
    return ctaskqueueSchedule(ctq, task, delay);
}
// bool tqSchedule(TaskQueue *tq, ComplexTask *task, int64 delay);
// Add a task to the queue to run after a delay. Requires a complex task.
#define tqSchedule(tq, task, delay) _tqSchedule(tq, ComplexTask(task), delay)

_meta_inline bool _tqDefer(_Inout_ TaskQueue* tq, _In_ ComplexTask* task)
{
    ComplexTaskQueue* ctq = objDynCast(ComplexTaskQueue, tq);
    if (!ctq)
        return false;
    return ctaskqueueDefer(ctq, task);
}
// bool tqDefer(TaskQueue *tq, ComplexTask *task);
// Add a task to the queue to be held indefinitely until advanced with taskAdvance.
#define tqDefer(tq, task) _tqDefer(tq, ComplexTask(task))

// generic callback mechanism for basic use that doesn't need to create classes
typedef bool (*UserTaskCB)(TaskQueue *tq, void *data);

// Runs a custom function on a thread in a task queue's worker pool
bool tqCall(_Inout_ TaskQueue *tq, _In_ UserTaskCB func, _In_opt_ void *userdata);

// Returns the current number of worker threads
int32 tqWorkers(_In_ TaskQueue *tq);

_meta_inline uint32 _btaskState(BasicTask *bt)
{
    return atomicLoad(uint32, &bt->state, Acquire) & TASK_State_Mask;
}
#define btaskState(task) _btaskState(BasicTask(task))
#define taskState(task) _btaskState(BasicTask(task))

_meta_inline bool _btaskIsRunning(BasicTask *bt)
{
    return _btaskState(bt) == TASK_Running;
}
#define btaskIsRunning(task) _btaskIsRunning(BasicTask(task))
#define taskIsRunning(task) _btaskIsRunning(BasicTask(task))

_meta_inline bool _btaskIsPending(BasicTask *bt)
{
    uint32 state = _btaskState(bt);
    return state == TASK_Waiting || state == TASK_Deferred || state == TASK_Scheduled;
}
#define btaskIsPending(task) _btaskIsPending(BasicTask(task))
#define taskIsPending(task) _btaskIsPending(BasicTask(task))

_meta_inline bool _btaskIsIdle(BasicTask *bt)
{
    uint32 state = _btaskState(bt);
    return state == TASK_Created || state == TASK_Waiting || state == TASK_Deferred ||
        state == TASK_Scheduled;
}
#define btaskIsIdle(task) _btaskIsIdle(BasicTask(task))
#define taskIsIdle(task) _btaskIsIdle(BasicTask(task))

_meta_inline bool _btaskIsComplete(BasicTask* bt)
{
    uint32 state = _btaskState(bt);
    return state == TASK_Succeeded || state == TASK_Failed;
}
#define btaskIsComplete(task) _btaskIsComplete(BasicTask(task))
#define taskIsComplete(task) _btaskIsComplete(BasicTask(task))

_meta_inline bool _btaskSucceeded(BasicTask *bt)
{
    return _btaskState(bt) == TASK_Succeeded;
}
#define btaskSucceeded(task) _btaskSucceeded(BasicTask(task))
#define taskSucceeded(task) _btaskSucceeded(BasicTask(task))

_meta_inline bool _btaskFailed(BasicTask *bt)
{
    return _btaskState(bt) == TASK_Failed;
}
#define btaskFailed(task) _btaskFailed(BasicTask(task))
#define taskFailed(task) _btaskFailed(BasicTask(task))

_meta_inline bool _btaskCancelled(BasicTask *bt)
{
    return atomicLoad(uint32, &bt->state, Acquire) & TASK_Cancelled;
}
#define btaskCancelled(task) _btaskCancelled(BasicTask(task))
#define taskCancelled(task) _btaskCancelled(BasicTask(task))

#define taskCancel(task) btaskCancel(task)

#define taskWait(task, timeout) ftaskWait(task, timeout)
