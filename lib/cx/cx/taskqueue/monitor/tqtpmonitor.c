// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tqtpmonitor.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "cx/taskqueue/taskqueue_private.h"
#include <cx/format.h>
#include <cx/log.h>

_objfactory_guaranteed TQThreadPoolMonitor*
TQThreadPoolMonitor_create(_In_ TaskQueueMonitorConfig* config)
{
    TQThreadPoolMonitor* self;
    self = objInstCreate(TQThreadPoolMonitor);

    self->conf = *config;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool TQThreadPoolMonitor_init(_In_ TQThreadPoolMonitor* self)
{
    // Autogen begins -----
    mutexInit(&self->monitorlock);
    return true;
    // Autogen ends -------
}

static uint16 ptrHash(void* ptr)
{
    uint16 ret = 0;
    uint8* p   = (uint8*)&ptr;
    for (int i = 0; i < sizeof(void*); i += 2, p += 2) {
        ret ^= ((uint16)p[0] << 8) | p[1];
    }
    return ret;
}

static void dumpTask(LogCategory* cat, void* ptr, int64 now, int wnum, bool progresstime,
                     bool ismanager)
{
    if (!ptr) {
        if (wnum > -1) {
            if (ismanager)
                logFmtC(Diag, cat, _S"    ${int}: Manager", stvar(int32, wnum + 1));
            else
                logFmtC(Diag, cat, _S"    ${int}: Idle", stvar(int32, wnum + 1));
        }
        return;
    }

    BasicTask* btask   = ptr;
    Task* task         = objDynCast(Task, btask);
    ComplexTask* ctask = objDynCast(ComplexTask, btask);
    if (progresstime && !ctask)
        return;

    string state = _S"Unknown";
    switch (btaskState(btask)) {
    case TASK_Created:
        state = _S"Created";
        break;
    case TASK_Running:
        state = _S"Running";
        break;
    case TASK_Waiting:
        state = _S"Waiting";
        break;
    case TASK_Deferred:
        state = _S"Deferred";
        break;
    case TASK_Succeeded:
        state = _S"Succeeded";
        break;
    case TASK_Failed:
        state = _S"Failed";
        break;
    case TASK_Scheduled:
        if (ctask) {
            if (ctask->nextrun == -1)
                state = _S"Scheduled:NextTask";
            else if (ctask->nextrun == 0)
                state = _S"Scheduled:Immediately";
            else if (ctask->nextrun == timeForever)
                state = _S"Scheduled:Never";
            else {
                strTemp(&state, 32);
                strFormat(&state,
                          _S"Scheduled:${int}ms",
                          stvar(int32, (int32)timeToMsec(clamplow(ctask->nextrun - now, 0))));
            }
        } else {
            state = _S"Scheduled";
        }
        break;
    }

    string prefix = 0;
    string suffix = 0;
    strTemp(&prefix, 16);
    strTemp(&suffix, 24);

    if (wnum > -1) {
        strFormat(&prefix, _S"${int}: ", stvar(int32, wnum + 1));
    }

    if (task && task->last > 0) {
        strFormat(&suffix,
                  _S" [${int}ms]",
                  stvar(int64,
                        progresstime ? timeToMsec(now - ctask->lastprogress) :
                                       timeToMsec(now - task->last)));
    }

    logFmtC(Diag,
            cat,
            _S"    ${string}${string}-${0uint(4,hex)} (${string})${string}",
            stvar(strref, prefix),
            stvar(strref, task ? task->name : _S"BasicTask"),
            stvar(uint16, ptrHash(ptr)),
            stvar(strref, state),
            stvar(strref, suffix));

    strDestroy(&state);
    strDestroy(&prefix);
    strDestroy(&suffix);
}

static void dumpPRQ(LogCategory* cat, PrQueue* prq, int64 now)
{
    uint32 qcount = prqCount(prq);
    for (uint32 ui = 0; ui < qcount; ui++) {
        dumpTask(cat, prqPeek(prq, ui), now, -1, false, false);
    }
}

static void dumpQueue(TQThreadPoolMonitor* self, TaskQueue* tq, TQThreadPoolRunner* runner,
                      int64 now)
{
    TaskQueueMonitorConfig* c = &self->conf;

    logStrC(Diag, c->mLogCat, _S"  Worker Threads:");
    rwlockAcquireRead(&runner->workerlock);
    int32 nworkers = saSize(runner->workers);
    for (int i = 0; i < nworkers; i++) {
        dumpTask(c->mLogCat,
                 atomicLoad(ptr, &runner->workers.a[i]->curtask, Relaxed),
                 now,
                 i,
                 false,
                 runner->workers.a[i]->thr == thrCurrent());
    }
    rwlockReleaseRead(&runner->workerlock);

    logStrC(Diag, c->mLogCat, _S"  Run Queue:");
    dumpPRQ(c->mLogCat, &tq->runq, now);
    logStrC(Diag, c->mLogCat, _S"  Done Queue:");
    dumpPRQ(c->mLogCat, &tq->doneq, now);

    ComplexTaskQueue* ctq = objDynCast(ComplexTaskQueue, tq);
    if (ctq) {
        if (saSize(ctq->scheduled) > 0) {
            logStrC(Diag, c->mLogCat, _S"  Scheduled:");
            for (int i = 0, ndefer = saSize(ctq->scheduled); i < ndefer; i++) {
                dumpTask(c->mLogCat, ctq->scheduled.a[i], now, -1, true, false);
            }
        }
        if (htSize(ctq->deferred) > 0) {
            logStrC(Diag, c->mLogCat, _S"  Deferred:");
            foreach (hashtable, hti, ctq->deferred) {
                dumpTask(c->mLogCat, htiKey(object, hti), now, -1, true, false);
            }
        }
    }
}

static void doWarn(TQThreadPoolMonitor* self, TaskQueue* tq, bool* warned)
{
    if (*warned)
        return;

    logBatchBegin();
    logFmtC(Warn, self->conf.mLogCat, _S"Task Queue Monitor (${string}):", stvar(string, tq->name));

    *warned = true;
}

bool TQThreadPoolMonitor_start(_In_ TQThreadPoolMonitor* self, _In_ TaskQueue* tq)
{
    self->tq      = objAcquire(tq);
    self->lastrun = clockTimer();
    return true;
}

int64 TQThreadPoolMonitor_tick(_In_ TQThreadPoolMonitor* self)
{
    // monitor runs from the manager thread; so we can safely access all
    // the tasks and structures without locking
    TaskQueueMonitorConfig* c = &self->conf;
    int64 now                 = clockTimer();

    if (c->mInterval == 0)
        return timeForever;

    if (now - self->lastwarn < c->mSuppress)
        return c->mSuppress - (now - self->lastwarn);

    if (now - self->lastrun < c->mInterval)
        return c->mInterval - (now - self->lastrun);

    mutexAcquire(&self->monitorlock);

    TaskQueue* tq              = self->tq;
    TQThreadPoolRunner* runner = tq ? objDynCast(TQThreadPoolRunner, tq->runner) : NULL;

    if (!runner) {
        mutexRelease(&self->monitorlock);
        return timeForever;
    }

    self->lastrun = now;

    bool warned = false;
    bool dumpq  = false;

    // check worker threads for tasks that have been running too long
    rwlockAcquireRead(&runner->workerlock);
    int32 nworkers = saSize(runner->workers);
    for (int i = 0; i < nworkers; i++) {
        void* ptr  = atomicLoad(ptr, &runner->workers.a[i]->curtask, Relaxed);
        Task* task = objDynCast(Task, (BasicTask*)ptr);
        if (task && now > task->last + c->mTaskRunning) {
            doWarn(self, tq, &warned);
            logFmtC(Warn,
                    c->mLogCat,
                    _S"  ${string}-${0uint(4,hex)} running for ${int} ms",
                    stvar(strref, task->name),
                    stvar(uint16, ptrHash(task)),
                    stvar(int64, timeToMsec(now - task->last)));
        }
    }
    rwlockReleaseRead(&runner->workerlock);

    // check the run queue for tasks that have been there too long
    uint32 qcount = prqCount(&tq->runq);
    for (uint32 ui = 0; ui < qcount; ui++) {
        void* ptr = prqPeek(&tq->runq, ui);
        if (ptr) {
            Task* task = objDynCast(Task, (BasicTask*)ptr);
            if (task && now > task->last + c->mTaskWaiting && !dumpq) {
                doWarn(self, tq, &warned);
                dumpq = true;
                logFmtC(Warn,
                        c->mLogCat,
                        _S"  ${string}-${0uint(4,hex)} waiting for ${int} ms",
                        stvar(strref, task->name),
                        stvar(uint16, ptrHash(task)),
                        stvar(int64, timeToMsec(now - task->last)));
            }
        }
    }

    ComplexTaskQueue* ctq = objDynCast(ComplexTaskQueue, tq);
    if (ctq) {
        // check the scheduled task list for tasks that aren't scheduled and aren't making progress
        for (int i = saSize(ctq->scheduled) - 1; i >= 0 && !dumpq; --i) {
            ComplexTask* ctask = ctq->scheduled.a[i];
            if (ctask && (ctask->nextrun == timeForever || ctask->nextrun == -1) &&
                now > ctask->lastprogress + c->mTaskStalled) {
                doWarn(self, tq, &warned);
                dumpq = true;
                logFmtC(Warn,
                        c->mLogCat,
                        _S"  ${string}-${0uint(4,hex)} stalled for ${int} ms",
                        stvar(strref, ctask->name),
                        stvar(uint16, ptrHash(ctask)),
                        stvar(int64, timeToMsec(now - ctask->lastprogress)));
                break;
            }
        }

        // check the defer queue for tasks that aren't making progress
        foreach (hashtable, hti, ctq->deferred) {
            if (dumpq)
                break;

            ComplexTask* ctask = (ComplexTask*)htiKey(object, hti);
            if (ctask && now > ctask->lastprogress + c->mTaskStalled) {
                doWarn(self, tq, &warned);
                dumpq = true;
                logFmtC(Warn,
                        c->mLogCat,
                        _S"  ${string}-${0uint(4,hex)} stalled for ${int} ms",
                        stvar(strref, ctask->name),
                        stvar(uint16, ptrHash(ctask)),
                        stvar(int64, timeToMsec(now - ctask->lastprogress)));
                break;
            }
        }
    }

    if (dumpq)
        dumpQueue(self, tq, runner, now);

    if (warned) {
        logBatchEnd();
        self->lastwarn = now;
    }

    mutexRelease(&self->monitorlock);

    return c->mInterval;
}

bool TQThreadPoolMonitor_stop(_In_ TQThreadPoolMonitor* self)
{
    if (!self->tq)
        return false;

    withMutex (&self->monitorlock) {
        objRelease(&self->tq);
    }
    return true;
}

void TQThreadPoolMonitor_destroy(_In_ TQThreadPoolMonitor* self)
{
    // Autogen begins -----
    mutexDestroy(&self->monitorlock);
    // Autogen ends -------
}

// Autogen begins -----
#include "tqtpmonitor.auto.inc"
// Autogen ends -------
