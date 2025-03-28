// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqthreadworker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"
#include <cx/format.h>

static int tqWorkerThread(Thread* thr)
{
    TQThreadWorker* self = stvlNextObj(&thr->args, TQThreadWorker);
    TaskQueue* tq        = stvlNextObj(&thr->args, TaskQueue);

    if (!(self && tq))
        return 1;

    // optional UI parameter, this is not allowed to change once the queue is configured
    self->ui = stvlNextPtr(&thr->args);

    tqthreadworkerOnStart(self, tq);

    while (thrLoop(thr)) {
        uint32 qstate = atomicLoad(uint32, &tq->state, Acquire);
        int64 maxwait = timeForever;
        if (qstate == TQState_Running)
            maxwait = tqthreadworkerTick(self, tq);
        else if (qstate == TQState_Stopping || qstate == TQState_Shutdown)
            break;

        if (maxwait > 0)
            eventWaitTimeout(&tq->workev, maxwait);
    }

    tqthreadworkerOnStop(self, tq);

    // the runner might be waiting on us to exit
    self->shutdown             = true;
    TQThreadPoolRunner* runner = objDynCast(TQThreadPoolRunner, tq->runner);
    if (runner)
        eventSignal(&runner->workershutdown);

    return 0;
}

_objfactory_guaranteed TQThreadWorker* TQThreadWorker_create(int32 num)
{
    TQThreadWorker* self;
    self = objInstCreate(TQThreadWorker);

    self->num = num;

    objInstInit(self);

    return self;
}

bool TQThreadWorker_startThread(_In_ TQThreadWorker* self, _In_ TaskQueue* tq)
{
    TQThreadPoolRunner* runner = objDynCast(TQThreadPoolRunner, tq->runner);
    if (!runner)
        return false;

    string thrname = 0;
    strFormat(&thrname,
              _S"${string} Worker #${int}",
              stvar(string, tq->name),
              stvar(int32, self->num));

    // may need to create a UI thread for the worker if there's a callback
    if (runner->conf.ui)
        self->thr = thrCreateUI(tqWorkerThread,
                                thrname,
                                stvar(object, self),
                                stvar(object, tq),
                                stvar(ptr, runner->conf.ui));
    else
        self->thr = thrCreate(tqWorkerThread, thrname, stvar(object, self), stvar(object, tq));

    strDestroy(&thrname);
    return self->thr;
}

int64 TQThreadWorker_tick(_In_ TQThreadWorker* self, _In_ TaskQueue* tq)
{
    BasicTask* btask;
    int64 waittime  = timeForever;
    bool workertick = false;

    // Some managers need to do things like adjust pool size before we run any tasks.
    if (tq->manager->needsWorkerTick)
        tqmanagerPretask(tq->manager);

    while ((btask = (BasicTask*)prqPop(&tq->runq))) {
        // Do UI stuff first if this is a UI task queue, to keep things responsive
        if (self->ui)
            self->ui(tq);

        // IMPLEMENTATION NOTE: At this point we become the owner of the btask pointer
        // and it does not exist anywhere else in the queue -- MOSTLY. We are not allowed
        // to destroy the object because the manager/monitor thread may be accessing it
        // concurrently through our curtask pointer.

        // See if this is a full Task or just a BasicTask
        Task* task = objDynCast(Task, btask);

        if (task)
            task->last = clockTimer();

        // run it
        atomicStore(ptr, &self->curtask, btask, Release);
        taskqueue_runTask(tq, &btask, self);
        atomicStore(ptr, &self->curtask, NULL, Release);

        // For in-worker managers, we need to tick the manager
        if (tq->manager->needsWorkerTick) {
            int64 mgrtime = tqmanagerTick(tq->manager);
            waittime      = min(waittime, mgrtime);
            workertick    = true;
        }
    }

    // Only tick the manager here if we didn't have any tasks to process
    if (tq->manager->needsWorkerTick && !workertick) {
        int64 mgrtime = tqmanagerTick(tq->manager);
        waittime      = min(waittime, mgrtime);
    }

    // If we're a UI thread see if there are STILL unprocessed UI events, and if so, ask to be run
    // again immediately.
    if (self->ui && !self->ui(tq))
        return 0;

    return waittime;
}

void TQThreadWorker_destroy(_In_ TQThreadWorker* self)
{
    // Autogen begins -----
    objRelease(&self->thr);
    // Autogen ends -------
}

// Autogen begins -----
#include "tqthreadworker.auto.inc"
// Autogen ends -------
