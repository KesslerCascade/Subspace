// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqcomplex.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

_objfactory_guaranteed ComplexTaskQueue* ComplexTaskQueue_create(_In_opt_ strref name, uint32 flags, int64 gcinterval, _In_ TQRunner* runner, _In_ TQManager* manager, _In_opt_ TQMonitor* monitor)
{
    ComplexTaskQueue* self;
    self = objInstCreate(ComplexTaskQueue);

    strDup(&self->name, name);
    self->flags      = flags;
    self->gcinterval = gcinterval;
    self->runner     = objAcquire(runner);
    self->manager    = objAcquire(manager);
    self->monitor    = objAcquire(monitor);
    objInstInit(self);

    return self;
}

_objinit_guaranteed bool ComplexTaskQueue_init(_In_ ComplexTaskQueue* self)
{
    prqInitDynamic(&self->advanceq, 4, 16, 0, PRQ_Grow_100, PRQ_Grow_100);
    self->advanceq.shrinkms = 5;

    // Autogen begins -----
    saInit(&self->scheduled, object, 1, SA_Ref | SA_Sorted);
    htInit(&self->deferred, object, none, 16, HT_RefKeys);
    return true;
    // Autogen ends -------
}

static bool CTQPushBackCommon(_Inout_ ComplexTaskQueue* self, _In_ ComplexTask* task, uint32 state,
                              int64 delay)
{
    if (atomicLoad(uint32, &self->state, Relaxed) != TQState_Running)
        return false;

    // only freshly created or reset tasks are legal to add to a queue - that includes moving them
    // directly to scheduled / deferred status
    if (btaskState(task) != TASK_Created)
        return false;

    // try to move it to desired state
    if (!ctask_setState(task, state))
        return false;

    // add a reference count which becomes owned by the queue
    objAcquire(task);
    task->last    = clockTimer();   // record when it was put in queue
    task->nextrun = (state == TASK_Scheduled && delay != -1) ? task->last + delay : 0;

    // remember that it's associated with this queue for when it's advanced
    task->lastq = objGetWeak(ComplexTaskQueue, self);

    // By putting it in the done queue, the manager thread will pick it up and stick it in the
    // scheduled / deferred list, which we can't safely access from other threads.
    if (!prqPush(&self->doneq, task)) {
        ctask_setState(task, TASK_Failed);
        objRelease(&task);
        return false;
    }

    // Notify the manager to do something with it
    tqmanagerNotify(self->manager, true);
    return true;
}

bool ComplexTaskQueue_schedule(_In_ ComplexTaskQueue* self, _In_ ComplexTask* task, int64 delay)
{
    return CTQPushBackCommon(self, task, TASK_Scheduled, delay);
}

bool ComplexTaskQueue_defer(_In_ ComplexTaskQueue* self, _In_ ComplexTask* task)
{
    return CTQPushBackCommon(self, task, TASK_Deferred, 0);
}

extern bool TaskQueue__processDone(_In_ TaskQueue* self);   // parent
#define parent__processDone() TaskQueue__processDone((TaskQueue*)(self))
bool ComplexTaskQueue__processDone(_In_ ComplexTaskQueue* self)
{
    int64 now = clockTimer();
    bool ret  = false;
    int dcount = 0;

    BasicTask* btask;
    while ((btask = (BasicTask*)prqPop(&self->doneq))) {
        uint32 state = btaskState(btask);
        switch (state) {
        case TASK_Succeeded:
        case TASK_Failed:
            // task is complete, remove it from the queue and possibly destroy it
            objRelease(&btask);
            ret = true;
            break;
        case TASK_Scheduled:
        case TASK_Deferred: {
            ComplexTask* ctask = objDynCast(ComplexTask, btask);

            if (!devVerify(ctask)) {
                btask_setState(btask, TASK_Failed);
                objRelease(&btask);
                break;
            }

            if (ctask->lastprogress == 0)
                ctask->lastprogress = now;
            ctask->last = now;

            if (state == TASK_Scheduled) {
                saPush(&self->scheduled, object, ctask);
            } else {
                htInsert(&self->deferred, object, ctask, none, NULL);
            }

            // if this task was advanced in the interim, ensure that it gets processed again
            if (atomicLoad(uint32, &ctask->_advcount, Relaxed) > 0)
                prqPush(&self->advanceq, ctask);

            break;
        }
        case TASK_Waiting:
            // A waiting task can end up in the doneq if it's a task that completed as deferred was
            // immediately advanced, all while the manager was in between processDone and
            // processExtra. In that case, just move it back to the run queue.
            prqPush(&self->runq, btask);
            ++dcount;
            break;
        default:
            // task shouldn't be in the doneq in some other state
            btask_setState(btask, TASK_Failed);
            objRelease(&btask);
        }
    }

    // if anything went back into the run queue, signal the event so that worker threads can pick it up
    if (dcount > 0)
        eventSignalMany(&self->workev, dcount);

    // return true if some tasks were completed, false otherwise
    return ret;
}

extern int64 TaskQueue__processExtra(_In_ TaskQueue* self, bool taskscompleted);   // parent
#define parent__processExtra(taskscompleted) TaskQueue__processExtra((TaskQueue*)(self), taskscompleted)
int64 ComplexTaskQueue__processExtra(_In_ ComplexTaskQueue* self, bool taskscompleted)
{
    int64 waittime = timeForever;
    int64 now      = clockTimer();
    sa_ComplexTask readvance = saInitNone;

    int dcount = 0;
    // process advance queue
    ComplexTask* ctask;
    while ((ctask = (ComplexTask*)prqPop(&self->advanceq))) {
        AdaptiveSpinState astate = { 0 };
        // try to consume one of the advcount entries
        uint32 advcount          = atomicLoad(uint32, &ctask->_advcount, Relaxed);
        while (advcount > 0 &&
               !atomicCompareExchange(uint32,
                                      weak,
                                      &ctask->_advcount,
                                      &advcount,
                                      advcount - 1,
                                      Relaxed,
                                      Relaxed)) {
            aspinHandleContention(NULL, &astate);
        }
        if (advcount == 0)
            continue;   // probably got added back into advanceq but was already handled

        uint32 state = taskState(ctask);
        switch (state) {
        case TASK_Scheduled:
            ctask_setState(ctask, TASK_Waiting);
            if (saFindRemove(&self->scheduled, object, ctask)) {
                ctask->last = now;
                prqPush(&self->runq, ctask);
                ++dcount;
            } else {
                // might have just been put in doneq? Let it run ASAP if that's the case
                ctask->nextrun = 0;
                waittime       = 0;
            }
            break;
        case TASK_Deferred:
            ctask_setState(ctask, TASK_Waiting);
            if (htRemove(&self->deferred, object, ctask)) {
                ctask->last = now;
                prqPush(&self->runq, ctask);
                ++dcount;
            } else {
                // might have just been put in doneq? Let it run ASAP if that's the case
                ctask->nextrun = 0;
                waittime = 0;
            }
            break;
        case TASK_Failed:
            // A failed task can be advanced if a dependency fails; try to move these straight to
            // doneq from wherever they are.
            if (htRemove(&self->deferred, object, ctask) ||
                saFindRemove(&self->scheduled, object, ctask)) {
                prqPush(&self->doneq, ctask);
                waittime = 0;
            }
            break;
        default:
            // somebody tried to advance a task that isn't deferred or scheduled, just ignore it
            break;
        }
    }

    // process scheduled list
    for (int i = 0, imax = saSize(self->scheduled); i < imax; ++i) {
        ctask         = self->scheduled.a[i];
        int64 nextrun = ctask->nextrun;

        // nextrun of -1 is special and means the task is scheduled to run whenever any other task
        // completes
        if ((nextrun != -1 || taskscompleted) && nextrun <= now) {
            // time for this task to run, so move it back to runq
            ctask_setState(ctask, TASK_Waiting);
            ctask->last = now;
            saRemove(&self->scheduled, i);
            prqPush(&self->runq, ctask);
            --i;
            --imax;
            ++dcount;
        }

        // the list is sorted, so once we hit one with a time in the future, just stop
        if (nextrun > now) {
            // but make sure we don't wait too long
            waittime = min(waittime, nextrun - now);
            break;
        }
    }

    // if we put anything back in, signal the event so that worker threads can pick it up
    if (dcount > 0)
        eventSignalMany(&self->workev, dcount);

    foreach(sarray, idx, void*, ctaskptr, readvance) {
        prqPush(&self->advanceq, ctaskptr);
    }
    saDestroy(&readvance);

    return waittime;
}

bool ComplexTaskQueue_advance(_In_ ComplexTaskQueue* self, _In_ ComplexTask* task)
{
    uint32 state = taskState(task);
    if (atomicLoad(uint32, &self->state, Relaxed) != TQState_Running ||
        !(state == TASK_Deferred || state == TASK_Scheduled))
        return false;

    // Put it in the advance queue, manager will safely move it out of the underlying structure,
    // either in a dedicated thread or with a lock held
    if (!prqPush(&self->advanceq, task))
        return false;

    // Signal the manager to move it
    tqmanagerNotify(self->manager, true);
    return true;
}

void ComplexTaskQueue_destroy(_In_ ComplexTaskQueue* self)
{
    ctaskqueue_clear(self);

    prqDestroy(&self->advanceq);

    // Autogen begins -----
    saDestroy(&self->scheduled);
    htDestroy(&self->deferred);
    // Autogen ends -------
}

extern void TaskQueue__clear(_In_ TaskQueue* self);   // parent
#define parent__clear() TaskQueue__clear((TaskQueue*)(self))
void ComplexTaskQueue__clear(_In_ ComplexTaskQueue* self)
{
    parent__clear();

    ComplexTask* ctask;
    while ((ctask = prqPop(&self->advanceq))) {}

    for (int i = saSize(self->scheduled) - 1; i >= 0; --i) {
        objRelease(&self->scheduled.a[i]);
    }
    saClear(&self->scheduled);

    foreach (hashtable, it, self->deferred) {
        ctask = (ComplexTask*)htiKey(object, it);
        objRelease(&ctask);
    }
    htClear(&self->deferred);
}

extern bool TaskQueue__runTask(_In_ TaskQueue* self, _Inout_ BasicTask** pbtask, _In_ TQWorker* worker);   // parent
#define parent__runTask(pbtask, worker) TaskQueue__runTask((TaskQueue*)(self), pbtask, worker)
bool ComplexTaskQueue__runTask(_In_ ComplexTaskQueue* self, _Inout_ BasicTask** pbtask, _In_ TQWorker* worker)
{
    // context: This is called from worker threads. The task pointed to by pbtask has been removed
    // from the run queue and exists only in the parameter; so we MUST do something with it.
    bool completed           = false;
    bool cancelled           = btaskCancelled(*pbtask);
    ComplexTask* ctask       = objDynCast(ComplexTask, *pbtask);
    sa_TaskRequires acquired = saInitNone;

    // If this is a complex task, check the dependencies. This is also where we try to acquire any
    // resources needed by the task.
    if (ctask && !cancelled) {
        int64 expires = timeForever;

        // Updating progress can make this more expensive if there are many dependencies, so only do
        // that if we have a monitor and need to care about progress.
        uint32 reqstate = ctaskCheckRequires(ctask, self->monitor != NULL, &expires);

        if (reqstate == TASK_Requires_Acquire) {
            // we know the size will be less than the number of requires, so preallocate enough
            saInit(&acquired, object, saSize(ctask->_requires), SA_Ref);
            if (ctaskAcquireRequires(ctask, &acquired)) {
                reqstate = TASK_Requires_Ok;
            } else {
                reqstate = TASK_Requires_Wait;

                if (!(ctask->flags & TASK_Retain_Requires)) {
                    // in the failure case, release them now unless the task wants to retain them
                    ctaskReleaseRequires(ctask, acquired);
                }
            }
        }

        if (reqstate == TASK_Requires_Wait) {
            // can't run it, defer it and send it to the doneq
            if (expires < timeForever) {
                ctask->nextrun = expires;
                btask_setState(*pbtask, TASK_Scheduled);
            } else {
                btask_setState(*pbtask, TASK_Deferred);
            }
            if (!prqPush(&self->doneq, *pbtask)) {
                ctask_setState(*pbtask, TASK_Failed);
                objRelease(pbtask);
            }

            // if this task was advanced in the interim, ensure that it gets processed again
            if (atomicLoad(uint32, &ctask->_advcount, Relaxed) > 0)
                prqPush(&self->advanceq, ctask);

            tqmanagerNotify(self->manager, !self->manager->needsWorkerTick);

            saDestroy(&acquired);
            *pbtask = NULL;
            return false;
        } else if (reqstate == TASK_Requires_Fail_Permanent) {
            cancelled = true;   // cancel the task if requirements can't be satisfied
        }
    }

    if (ctask && cancelled && (ctask->flags & TASK_Cancel_Cascade)) {
        // cancel any dependencies if we're in cascade mode
        ctaskCancelRequires(ctask);
    }

    if (!btask_setState(*pbtask, TASK_Running)) {
        btask_setState(*pbtask, TASK_Failed);
        objRelease(pbtask);
        return false;
    }

    TaskControl tcon = { 0 };
    uint32 tresult;
    if (!cancelled) {
        tresult = btaskRun(*pbtask, self, worker, &tcon);
    } else {
        // if the task has been cancelled, we still give it one chance to clean something up, then
        // automatically fail the task
        btaskRunCancelled(*pbtask, self, worker);
        tresult = TASK_Result_Failure;
    }

    Task* task = Task(ctask);
    int64 now  = clockTimer();

    if (ctask &&
        (tresult == TASK_Result_Schedule_Progress || tresult == TASK_Result_Defer_Progress)) {
        ctask->lastprogress = now;
    }

    // Failsafe check, if this isn't a complex task it's only allowed to return success or failure.
    // Vastly simplifies the logic below.
    if (!ctask && !(tresult == TASK_Result_Success || tresult == TASK_Result_Failure)) {
        tresult = TASK_Result_Failure;
    }

    switch (tresult) {
    case TASK_Result_Success:
        btask_setState(*pbtask, TASK_Succeeded);
        completed = true;
        break;
    case TASK_Result_Schedule:
        ctask->nextrun = (tcon.delay == -1) ? -1 : now + tcon.delay;
        btask_setState(*pbtask, TASK_Scheduled);
        break;
    case TASK_Result_Schedule_Progress:
        if (tcon.delay == 0) {
            // special case, if we made progress AND the desired wait time is 0, put it directly
            // back in the run queue
            ctask->last = now;
            btask_setState(*pbtask, TASK_Waiting);

            if ((ctask->_intflags & TASK_INTERNAL_Owns_Resources) &&
                !(ctask->flags & TASK_Retain_Requires)) {
                ctaskReleaseRequires(ctask, acquired);
            }
            saDestroy(&acquired);

            prqPush(&self->runq, ctask);
            *pbtask = NULL;   // task no longer belongs to worker
            return true;      // return early so it does NOT get put in doneq
        } else {
            ctask->nextrun = (tcon.delay == -1) ? -1 : now + tcon.delay;
            btask_setState(*pbtask, TASK_Scheduled);
        }
    case TASK_Result_Defer:
    case TASK_Result_Defer_Progress:
        btask_setState(*pbtask, TASK_Deferred);
        break;
    case TASK_Result_Failure:
    default:
        // catch-all in case of unexpected return status
        btask_setState(*pbtask, TASK_Failed);
        completed = true;
    }

    if (completed) {
        // If this isn't a ComplexTask it might still be a Task
        if (!task)
            task = objDynCast(Task, *pbtask);
        if (task && task->oncomplete)
            cchainCallOnce(&task->oncomplete, stvar(object, task));

        if (tcon.notifyev) {
            eventSignal(tcon.notifyev);
        }
    } else if (ctask) {
        ctask->last = now;
    }

    // Free resources if any were acquired, but not if the task wants to retain resources, unless
    // the task is complete.
    if (ctask && (ctask->_intflags & TASK_INTERNAL_Owns_Resources) &&
        (!(ctask->flags & TASK_Retain_Requires) ||
         (tresult == TASK_Result_Success || tresult == TASK_Result_Failure))) {
        // If we're not retaining requires, we have a list of exactly which requires acquired any.
        // If not, we have to just scan all requires and try to release.
        ctaskReleaseRequires(ctask,
                             !(ctask->flags & TASK_Retain_Requires) ? acquired : ctask->_requires);
    }
    saDestroy(&acquired);    

    // In all other cases the task needs to be moved to the done queue for the manager to clean up.
    prqPush(&self->doneq, *pbtask);
    *pbtask = NULL;   // task no longer belongs to worker
    // Manger needs to process doneq, but only wake it up if it's running in a separate thread. In
    // the in-worker case we're about to tick the manager anyway.
    tqmanagerNotify(self->manager, !self->manager->needsWorkerTick);

    return completed;
}

extern bool TaskQueue_add(_In_ TaskQueue* self, _In_ BasicTask* btask);   // parent
#define parent_add(btask) TaskQueue_add((TaskQueue*)(self), btask)
bool ComplexTaskQueue_add(_In_ ComplexTaskQueue* self, _In_ BasicTask* btask)
{
    if (atomicLoad(uint32, &self->state, Relaxed) != TQState_Running)
        return false;

    // only freshly created or reset tasks are legal to add to a queue
    if (btaskState(btask) != TASK_Created)
        return false;

    ComplexTask* ctask = objDynCast(ComplexTask, btask);
    if (ctask) {
        // complex tasks remember which queue they belong to, for defer / schedule scenarios
        ctask->lastq = objGetWeak(ComplexTaskQueue, self);
    }

    // try to move it to waiting state
    if (!btask_setState(btask, TASK_Waiting))
        return false;

    // add a reference count which becomes owned by the queue
    objAcquire(btask);

    Task* task = objDynCast(Task, btask);
    if (task)
        task->last = clockTimer();   // record when it was put in run queue

    if (!prqPush(&self->runq, btask)) {
        btask_setState(btask, TASK_Failed);
        objRelease(&btask);
        return false;
    }

    // Signal the runner to pick it up
    eventSignal(&self->workev);
    // let dedicated managers know they may need to check for thread pool expansion.
    // In-worker managers will be ticked anyway, so don't signal workev twice.
    tqmanagerNotify(self->manager, !self->manager->needsWorkerTick);

    return true;
}

extern bool TaskQueue__queueMaint(_In_ TaskQueue* self);   // parent
#define parent__queueMaint() TaskQueue__queueMaint((TaskQueue*)(self))
bool ComplexTaskQueue__queueMaint(_In_ ComplexTaskQueue* self)
{
    int64 now = clockTimer();

    if (now - self->_lastgc > self->gcinterval) {
        self->_lastgc = now;
        switch(self->_gccycle) {
            case 0:
                prqCollect(&self->runq);
                break;
            case 1:
                prqCollect(&self->doneq);
                break;
            case 2:
                prqCollect(&self->advanceq);
                break;
            }

        self->_gccycle = (self->_gccycle + 1) % 3;
    }

    return true;
}

// Autogen begins -----
#include "tqcomplex.auto.inc"
// Autogen ends -------
