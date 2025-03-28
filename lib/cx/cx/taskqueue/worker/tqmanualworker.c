// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqmanualworker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"

_objfactory_guaranteed TQManualWorker* TQManualWorker_create()
{
    TQManualWorker* self;
    self = objInstCreate(TQManualWorker);

    objInstInit(self);

    return self;
}

int64 TQManualWorker_tick(_In_ TQManualWorker* self, _In_ TaskQueue* tq)
{
    BasicTask* btask;
    int64 waittime  = timeForever;
    bool workertick = false;

    // Some managers need to do things like adjust pool size before we run any tasks.
    if (tq->manager->needsWorkerTick)
        tqmanagerPretask(tq->manager);

    while ((btask = (BasicTask*)prqPop(&tq->runq))) {
        // IMPLEMENTATION NOTE: At this point we become the owner of the btask pointer
        // and it does not exist anywhere else in the queue -- MOSTLY. We are not allowed
        // to destroy the object because the manager/monitor thread may be accessing it
        // concurrently through our curtask pointer.

        // See if this is a full Task or just a BasicTask
        Task* task = objDynCast(Task, btask);

        if (task)
            task->last = clockTimer();

        // run it
        taskqueue_runTask(tq, &btask, self);

        // For in-worker managers, we need to tick the manager
        if (tq->manager->needsWorkerTick) {
            int64 mgrtime = tqmanagerTick(tq->manager);
            waittime      = min(waittime, mgrtime);
            workertick    = true;
        }

        // If this is a one-shot queue, stop processing now.
        if (tq->flags & TQ_Oneshot)
            break;
    }

    // Only tick the manager here if we didn't have any tasks to process
    if (tq->manager->needsWorkerTick && !workertick) {
        int64 mgrtime = tqmanagerTick(tq->manager);
        waittime      = min(waittime, mgrtime);
    }

    // If there's still anything left in the run queue, ask politely to be run again
    if ((tq->flags & TQ_Oneshot) && prqCount(&tq->runq) > 0)
        return 0;

    return waittime;
}

// Autogen begins -----
#include "tqmanualworker.auto.inc"
// Autogen ends -------
