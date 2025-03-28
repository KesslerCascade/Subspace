#include "taskqueue_private.h"
#include <cx/platform/os.h>
#include <cx/time/clock.h>
#include "userfunctask.h"

_Use_decl_annotations_
void tqPresetSingle(TaskQueueConfig* tqconfig)
{
    TaskQueueThreadPoolConfig tpconfig = { .wInitial = 1, .wIdle = 1, .wBusy = 1, .wMax = 1 };
    *tqconfig                          = (TaskQueueConfig) { .pool = tpconfig, .mGC = timeS(1) };
}

_Use_decl_annotations_
void tqPresetMinimal(TaskQueueConfig* tqconfig)
{
    int ncpus = osPhysicalCPUs();

    TaskQueueThreadPoolConfig tpconfig = {
        .wInitial   = 1,
        .wIdle      = 1,
        .wBusy      = (ncpus > 2) ? (ncpus + 1) / 2 : 1,
        .wMax       = ncpus,
        .tIdle      = timeMS(1000),
        .tRampUp    = timeMS(50),
        .tRampDown  = timeMS(250),
        .loadFactor = 8,
    };
    *tqconfig = (TaskQueueConfig) { .pool = tpconfig, .mGC = timeMS(500) };
}

_Use_decl_annotations_
void tqPresetBalanced(TaskQueueConfig* tqconfig)
{
    int npcpus = osPhysicalCPUs();
    int nlcpus = osLogicalCPUs();

    TaskQueueThreadPoolConfig tpconfig = {
        .wInitial   = 1,
        .wIdle      = (npcpus >= 2) ? 2 : 1,
        .wBusy      = npcpus,
        .wMax       = nlcpus,
        .tIdle      = timeMS(2500),
        .tRampUp    = timeMS(20),
        .tRampDown  = timeMS(500),
        .loadFactor = 2,
    };
    *tqconfig = (TaskQueueConfig) { .pool = tpconfig, .mGC = timeMS(50) };
}

_Use_decl_annotations_
void tqPresetHeavy(TaskQueueConfig* tqconfig)
{
    int npcpus = osPhysicalCPUs();
    int nlcpus = osLogicalCPUs();

    TaskQueueThreadPoolConfig tpconfig = {
        .wInitial   = 2,
        .wIdle      = (npcpus >= 4) ? 4 : 2,
        .wBusy      = nlcpus,
        .wMax       = nlcpus + (nlcpus >> 1),
        .tIdle      = timeMS(5000),
        .tRampUp    = timeMS(10),
        .tRampDown  = timeMS(1000),
        .loadFactor = 2,
    };
    *tqconfig = (TaskQueueConfig) { .flags = TQ_ManagerThread, .pool = tpconfig, .mGC = timeMS(10) };
}

_Use_decl_annotations_
void tqPresetManual(TaskQueueConfig* tqconfig)
{
    *tqconfig = (TaskQueueConfig) { .flags = TQ_Manual, .mGC = timeMS(500) };
}

_Use_decl_annotations_
void tqEnableMonitor(TaskQueueConfig* tqconfig)
{
    tqconfig->monitor.mInterval    = timeS(5);
    tqconfig->monitor.mSuppress    = timeS(60);
    tqconfig->monitor.mTaskRunning = timeS(120);
    tqconfig->monitor.mTaskWaiting = timeS(30);
    tqconfig->monitor.mTaskStalled = timeS(60);
    tqconfig->flags |= TQ_Monitor;
}

_Use_decl_annotations_
TaskQueue* tqCreate(strref name, TaskQueueConfig* tqconfig)
{
    if (!(tqconfig->flags & TQ_Manual)) {
        tqconfig->pool.wMax       = clamplow(tqconfig->pool.wMax, 1);
        tqconfig->pool.wIdle      = clamp(tqconfig->pool.wIdle, 1, tqconfig->pool.wMax);
        tqconfig->pool.wInitial   = clamp(tqconfig->pool.wInitial,
                                        tqconfig->pool.wIdle,
                                        tqconfig->pool.wMax);
        tqconfig->pool.wBusy      = clamp(tqconfig->pool.wBusy,
                                     tqconfig->pool.wIdle,
                                     tqconfig->pool.wMax);
        tqconfig->pool.loadFactor = clamplow(tqconfig->pool.loadFactor, 1);
    }

    if (tqconfig->mGC == 0)
        tqconfig->mGC = timeMS(250);

    TQRunner* runner   = NULL;
    TQManager* manager = NULL;
    TQMonitor* monitor = NULL;

    if (tqconfig->flags & TQ_Manual) {
        runner  = TQRunner(tqmanualrunnerCreate());
        manager = TQManager(tqmanualmanagerCreate());
    } else {
        runner = TQRunner(tqthreadpoolrunnerCreate(&tqconfig->pool));
        if (tqconfig->flags & TQ_ManagerThread)
            manager = TQManager(tqdedicatedmanagerCreate());
        else
            manager = TQManager(tqinworkermanagerCreate());

        if (tqconfig->flags & TQ_Monitor)
            monitor = TQMonitor(tqthreadpoolmonitorCreate(&tqconfig->monitor));
    }

    devAssert(runner && manager);

    TaskQueue* tq = NULL;
    if (tqconfig->flags & TQ_NoComplex) {
        // Basic task queue
        tq = taskqueueCreate(strEmpty(name) ? _S"TaskQueue" : name,
                             tqconfig->flags,
                             tqconfig->mGC,
                             runner,
                             manager,
                             monitor);
    } else {
        // Complex task queue
        tq = TaskQueue(ctaskqueueCreate(strEmpty(name) ? _S"TaskQueue" : name,
                                        tqconfig->flags,
                                        tqconfig->mGC,
                                        runner,
                                        manager,
                                        monitor));
    }

    objRelease(&runner);
    objRelease(&manager);
    objRelease(&monitor);

    return tq;
}

_Use_decl_annotations_
bool tqCall(TaskQueue* tq, UserTaskCB func, void* userdata)
{
    UserFuncTask* task = userfunctaskCreate(func, userdata);
    bool ret           = tqAdd(tq, task);
    objRelease(&task);
    return ret;
}

int32 tqWorkers(_In_ TaskQueue* tq)
{
    TQThreadPoolRunner* runner = objDynCast(TQThreadPoolRunner, tq->runner);
    if (!runner)
        return 0;

    int ret = 0;
    withReadLock (&runner->workerlock) {
        ret = saSize(runner->workers);
    }
    return ret;
}
