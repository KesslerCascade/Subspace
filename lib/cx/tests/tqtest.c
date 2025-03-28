#include <stdio.h>
#include <cx/string.h>
#include <cx/taskqueue.h>
#include <cx/log/logmembuf.h>
#include <cx/container/foreach.h>
#include <cx/taskqueue/requires/taskrequiresgate.h>
#include <cx/taskqueue/resource/trmutex.h>
#include <cx/taskqueue/resource/trfifo.h>
#include <cx/taskqueue/resource/trlifo.h>
#include "tqtestobj.h"

#define TEST_FILE tqtest
#define TEST_FUNCS tqtest_funcs
#include "common.h"

static atomic(intptr) accum1;
static atomic(intptr) accum2;
static Event notifyev;

#define NUM_TASK_TEST 50000

static int test_tqtest_task(void)
{
    int ret = 0;
    int total = 0, total2 = 0;
    sa_TQTest1 ttasks;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    conf.flags |= TQ_NoComplex;
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    saInit(&ttasks, object, NUM_TASK_TEST);

    eventInit(&notifyev);

    // create tasks
    for(int i = 0; i < NUM_TASK_TEST; i++) {
        int n1 = rand(), n2 = rand();
        total += n1 + n2;
        TQTest1 *t = tqtest1Create(n1, n2, &notifyev);
        if(t) {
            saPushC(&ttasks, object, &t);
        } else {
            ret = 1;
        }
    }

    // run tasks
    int ntasks = saSize(ttasks);
    for(int i = 0; i < ntasks; i++) {
        tqAdd(q, ttasks.a[i]);
    }

    // wait for tasks
    int64 timeStart = clockTimer();
    for(;;) {
        bool done = true;
        for(int i = 0; i < ntasks; i++) {
            switch(btaskState(ttasks.a[i])) {
            case TASK_Succeeded:
                break;
            case TASK_Failed:
                ret = 1;
                break;
            default:
                done = false;
            }
        }

        if(done)
            break;

        eventWaitTimeout(&notifyev, timeS(3));
        if(clockTimer() - timeStart > timeS(60)) {
            ret = 1;
            break;
        }
    }

    eventDestroy(&notifyev);
    tqShutdown(q, timeS(60));
    tqRelease(&q);

    for(int i = 0; i < ntasks; i++) {
        if(btaskState(ttasks.a[i]) != TASK_Succeeded)
            ret = 1;
        total2 += ttasks.a[i]->total;
    }

    if(total != total2)
        ret = 1;

    saDestroy(&ttasks);

    return ret;
}

static int test_tqtest_failure(void)
{
    int ret = 0;
    int nsuc = 0, nfail = 0;
    sa_TQTestFail ttasks;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    conf.flags |= TQ_NoComplex;
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    saInit(&ttasks, object, NUM_TASK_TEST);

    eventInit(&notifyev);

    // create tasks
    for(int i = 0; i < NUM_TASK_TEST; i++) {
        TQTestFail *t = tqtestfailCreate(i, &notifyev);
        if(t) {
            saPushC(&ttasks, object, &t);
        } else {
            ret = 1;
        }
    }

    // run tasks
    int ntasks = saSize(ttasks);
    for(int i = 0; i < ntasks; i++) {
        tqAdd(q, ttasks.a[i]);
    }

    // wait for tasks
    int64 timeStart = clockTimer();
    for(;;) {
        bool done = true;
        for(int i = 0; i < ntasks; i++) {
            switch(btaskState(ttasks.a[i])) {
            case TASK_Succeeded:
            case TASK_Failed:
                break;
            default:
                done = false;
            }
        }

        if(done)
            break;

        eventWaitTimeout(&notifyev, timeS(3));
        if(clockTimer() - timeStart > timeS(60)) {
            ret = 1;
            break;
        }
    }

    tqShutdown(q, timeS(60));
    tqRelease(&q);
    eventDestroy(&notifyev);

    for(int i = 0; i < ntasks; i++) {
        if(btaskState(ttasks.a[i]) == TASK_Succeeded)
            nsuc++;
        else if(btaskState(ttasks.a[i]) == TASK_Failed)
            nfail++;
    }

    if(nsuc != ntasks / 2 || nfail != ntasks / 2)
        ret = 1;

    saDestroy(&ttasks);

    return ret;
}

static int test_tqtest_concurrency(bool dedicated)
{
    int ret = 0;
    int total = 0;
    int accum = 0;
    int count = 0;

    TaskQueueConfig conf = { 0 };
    conf.pool.wInitial = 8;
    conf.pool.wIdle      = 8;
    conf.pool.wBusy      = 16;
    conf.pool.wMax       = 64;
    conf.pool.tIdle      = timeS(1);
    conf.pool.tRampUp    = timeMS(2);
    conf.pool.tRampDown  = timeMS(50);
    conf.flags |= TQ_NoComplex | (dedicated ? TQ_ManagerThread : 0);
    TaskQueue* spmc = tqCreate(_S"SPMC Test", &conf);
    if(!spmc || !tqStart(spmc))
        return 1;

    conf.pool.wInitial = 1;
    conf.pool.wIdle    = 1;
    conf.pool.wBusy    = 1;
    conf.pool.wMax     = 1;
    conf.pool.tIdle    = 0;
    conf.pool.tRampUp  = 0;
    conf.pool.tRampDown = 0;
    TaskQueue *mpsc = tqCreate(_S"MPSC Test", &conf);
    if(!mpsc || !tqStart(mpsc))
        return 1;

    eventInit(&notifyev);

    // fire off some tasks into the input queue
    for(int i = 0; i < NUM_TASK_TEST; i++) {
        int n1 = rand(), n2 = rand();
        total += n1 + n2;
        TQTestCC1 *t = tqtestcc1Create(n1, n2, mpsc, &accum, &count, &notifyev);
        tqRun(spmc, &t);
    }

    // wait for tasks
    int64 timeStart = clockTimer();
    while(count < NUM_TASK_TEST) {
        eventWaitTimeout(&notifyev, timeS(3));
        if(clockTimer() - timeStart > timeS(60)) {
            ret = 1;
            break;
        }
    }

    eventDestroy(&notifyev);
    tqShutdown(spmc, timeS(60));
    tqRelease(&spmc);
    tqShutdown(mpsc, timeS(60));
    tqRelease(&mpsc);

    if(total != accum)
        ret = 1;

    return ret;
}

static int test_tqtest_concurrency_inworker(void)
{
    return test_tqtest_concurrency(false);
}

static int test_tqtest_concurrency_dedicated(void)
{
    return test_tqtest_concurrency(true);
}

static bool tqtest_callcb(TaskQueue *tq, void *data)
{
    atomicFetchAdd(intptr, &accum1, (intptr)data, Relaxed);
    atomicFetchAdd(intptr, &accum2, 1, AcqRel);
    eventSignal(&notifyev);
    return true;
}

#define NUM_CALL_TEST 50000

static int test_tqtest_call(void)
{
    TaskQueueConfig conf;
    int ret = 0;
    intptr total = 0;

    eventInit(&notifyev);
    atomicStore(intptr, &accum1, 0, Relaxed);
    atomicStore(intptr, &accum2, 0, Relaxed);

    tqPresetBalanced(&conf);
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    for(int i = 0; i < NUM_CALL_TEST; i++) {
        intptr rnum = rand() % 255;
        total += rnum;
        tqCall(q, tqtest_callcb, (void *)rnum);
    }

    int64 timeStart = clockTimer();
    while(atomicLoad(intptr, &accum2, Acquire) < NUM_CALL_TEST) {
        eventWaitTimeout(&notifyev, timeS(3));
        if(clockTimer() - timeStart > timeS(60)) {
            ret = 1;
            break;
        }
    }

    tqShutdown(q, timeS(60));

    if(atomicLoad(intptr, &accum1, Acquire) != total)
        ret = 1;

    tqRelease(&q);

    eventDestroy(&notifyev);

    return ret;
}

#define NUM_SCHED_STEPS 10

static int is_monitor_test = 0;
static LogCategory *moncat;
atomic(int32) tqtests_order;

static bool sched_oncomplete(stvlist *cvars, stvlist *args)
{
    atomic(int32) *nsucceed;
    Task *task;

    nsucceed = stvlNextPtr(cvars);
    task = stvlNextObj(args, Task);
    if(!(nsucceed && task))
        return false;

    if(btaskState(task) == TASK_Succeeded) {
        atomicFetchAdd(int32, nsucceed, 1, AcqRel);
    }

    return true;
}

static int test_tqtest_sched(void)
{
    int ret = 0;
    sa_TQTestSched dtasks;
    atomic(int32) nsucceed = atomicInit(0);

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    if(is_monitor_test) {
        conf.monitor.mInterval = timeMS(5);
        conf.monitor.mTaskStalled = timeMS(110);
        conf.monitor.mTaskRunning = timeMS(60);
        conf.monitor.mTaskWaiting = timeMS(20);
        conf.monitor.mSuppress    = timeMS(50);
        conf.monitor.mLogCat      = moncat;
        conf.flags |= TQ_Monitor | (is_monitor_test == 2 ? TQ_ManagerThread : 0);
    }
    conf.pool.wInitial = 4;
    conf.pool.wIdle    = 4;
    conf.pool.wBusy    = 4;
    conf.pool.wMax     = 4;
    TaskQueue *q = tqCreate(_S"Test", &conf);

    if(!q || !tqStart(q))
        return 1;

    saInit(&dtasks, object, NUM_SCHED_STEPS);

    atomicStore(int32, &tqtests_order, 0, Relaxed);
    eventInit(&notifyev);

    // create tasks
    int64 dtime = timeMS(20);
    for(int i = 0; i < NUM_SCHED_STEPS; i++) {
        TQTestS1 *s1 = tqtests1Create(i, dtime, &notifyev);
        TQTestS2 *s2 = tqtests2Create(s1, &notifyev);

        // completion callbacks
        cchainAttach(&s1->oncomplete, sched_oncomplete, stvar(ptr, &nsucceed));
        cchainAttach(&s2->oncomplete, sched_oncomplete, stvar(ptr, &nsucceed));

        saPushC(&dtasks, object, &s2);           // run them in reverse order to test inversion
        saPushC(&dtasks, object, &s1);
        dtime += timeMS(20);
    }

    TQDelayTest *dlt = tqdelaytestCreate(timeMS(150));
    tqRun(q, &dlt);

    // run tasks
    int ntasks = saSize(dtasks);
    for(int i = 0; i < ntasks; i++) {
        tqAdd(q, dtasks.a[i]);
    }

    // wait for tasks
    int64 timeStart = clockTimer();
    for(;;) {
        bool done = true;
        for(int i = 0; i < ntasks; i++) {
            switch(btaskState(dtasks.a[i])) {
            case TASK_Succeeded:
                break;
            case TASK_Failed:
                ret = 1;
                break;
            default:
                done = false;
            }
        }

        if(done)
            break;

        eventWaitTimeout(&notifyev, timeS(3));
        if(clockTimer() - timeStart > timeS(60)) {
            ret = 1;
            break;
        }
    }

    eventDestroy(&notifyev);
    tqShutdown(q, timeS(60));
    tqRelease(&q);

    for(int i = 0; i < ntasks; i++) {
        if(btaskState(dtasks.a[i]) != TASK_Succeeded)
            ret = 1;
    }

    if(atomicLoad(int32, &nsucceed, Acquire) != NUM_SCHED_STEPS * 2)
        ret = 1;

    saDestroy(&dtasks);

    return ret;
}

static int test_tqtest_monitor(bool dedicated)
{
    logRestart();   // only needed for alltests; shutdown may have previously been called
    LogMembufData *mbuf = logmembufCreate(65536);
    moncat = logCreateCat(_S"MonitorTest", true);
    logRegisterDest(LOG_Diag, moncat, logmembufDest, mbuf);

    // reuse the sched test, but with the monitor enabled
    is_monitor_test = dedicated ? 2 : 1;
    int ret = test_tqtest_sched();
    is_monitor_test = false;

    logFlush();

    if(mbuf->buf[0] == 0)
        ret = 1;

    logShutdown();

    return ret;
}

static int test_tqtest_monitor_inworker(void)
{
    return test_tqtest_monitor(false);
}

static int test_tqtest_monitor_dedicated(void)
{
    return test_tqtest_monitor(true);
}

static bool signal_oncomplete(stvlist* cvars, stvlist* args)
{
    Event *ev;

    ev = stvlNextPtr(cvars);
    if (!ev)
        return false;

    eventSignal(ev);
    return true;
}

#define NUM_DEPEND_TASKS 50000
#define NUM_DEPEND_TASKS2 250

// This also tests deferred tasks
static int test_tqtest_depend(void)
{
    Event notifyev;
    Event dummy;
    int ret = 0;

    atomicStore(int32, &tqtests_order, 0, Relaxed);
    eventInit(&notifyev);
    eventInit(&dummy);

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    conf.pool.wInitial = 2;
    conf.pool.wIdle    = 2;
    conf.pool.wBusy    = 3;
    conf.pool.wMax     = 16;
    TaskQueue* q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    TQMTest *tqmtest = tqmtestCreate(&notifyev);

    // create tasks
    int64 dtime = timeMS(20);
    for (int i = 0; i < NUM_SCHED_STEPS; i++) {
        TQTestS1 *d1 = tqtests1Create(i, dtime, &dummy);
        TQTestS2 *d2 = tqtests2Create(d1, &dummy);

        ctaskDependOn(tqmtest, d2);
        ctaskDependOn(tqmtest, d1);
        ctaskDependOn(d2, d1);
        tqAdd(q, d1);
        tqAdd(q, d2);
        objRelease(&d1);
        objRelease(&d2);

        dtime += timeMS(20);
    }

    // start half now, half after tqmtest
    sa_TQTest1 later;
    saInit(&later, object, NUM_DEPEND_TASKS / 2);
    for (int i = 0; i < NUM_DEPEND_TASKS; i++) {
        TQTest1 *t = tqtest1Create(1, 1, &dummy);
        ctaskDependOn(tqmtest, t);
        if ((i & 1) == 0)
            tqAdd(q, t);
        else
            saPush(&later, object, t);
        objRelease(&t);
    }

    tqAdd(q, tqmtest);

    foreach(sarray, idx, TQTest1*, lt, later)
    {
        tqAdd(q, lt);
    }
    saDestroy(&later);

    if(!eventWaitTimeout(&notifyev, timeS(60)))
        ret = 1;

    if(saSize(tqmtest->_requires) != 0)
        ret = 1;

    if(!taskSucceeded(tqmtest))
        ret = 1;

    objRelease(&tqmtest);

    // Test depending on a failed task
    tqmtest = tqmtestCreate(&dummy);
    cchainAttach(&tqmtest->oncomplete, signal_oncomplete, stvar(ptr, &notifyev));

    saInit(&later, object, NUM_DEPEND_TASKS * 2);
    for (int i = 0; i < NUM_DEPEND_TASKS2; i++) {
        TQTest1* t = tqtest1Create(1, 1, &dummy);
        ctaskDependOn(tqmtest, t);
        saPushC(&later, object, &t);
    }
    for (int i = 0; i < NUM_DEPEND_TASKS2; i++) {
        TQTestFail* t = tqtestfailCreate(i, &dummy);
        ctaskDependOn(tqmtest, t);
        saPushC(&later, object, &t);
    }

    tqAdd(q, tqmtest);

    foreach (sarray, idx, TQTest1*, lt, later) {
        tqAdd(q, lt);
    }
    saDestroy(&later);

    if (!eventWaitTimeout(&notifyev, timeS(60)))
        ret = 1;
    if (!taskFailed(tqmtest))
        ret = 1;

    objRelease(&tqmtest);

    eventDestroy(&dummy);
    eventDestroy(&notifyev);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;
}

#define NUM_REQUIRES_TASKS 10000
static ReqTestState rts;
static ReqTestState2 rts2;

static int test_tqtest_reqmutex(void)
{
    int ret = 0;

    memset(&rts, 0, sizeof(rts));
    rts.target_count = NUM_REQUIRES_TASKS;
    eventInit(&rts.notify);

    int sum = 0, xor = 0;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    TaskQueue* q       = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    TRMutex* trmtx = trmutexCreate();
    srand((unsigned int)time(NULL));

    for (int i = 0; i < NUM_REQUIRES_TASKS; i++) {
        int randval      = rand() % 256 + 1;
        sum += randval;
        xor ^= randval;
        TQRTestMtx* task = tqrtestmtxCreate(&rts, randval);
        ctaskRequireResource(task, trmtx);
        tqRun(q, &task);
    }

    if (!eventWaitTimeout(&rts.notify, timeS(60)))
        ret = 1;

    if (atomicLoad(bool, &rts.fail, Relaxed) || rts.count != NUM_REQUIRES_TASKS || rts.sum != sum ||
        rts.xor != xor)
        ret = 1;

    objRelease(&trmtx);
    eventDestroy(&rts.notify);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;
}

static int test_tqtest_reqfifo(void)
{
    int ret = 0;

    memset(&rts, 0, sizeof(rts));
    rts.target_count = NUM_REQUIRES_TASKS;
    rts.product      = 1;
    rts.seq          = -1;
    eventInit(&rts.notify);

    int sum = 0, product = 1;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    TaskQueue* q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    TRFifo* trfifo = trfifoCreate();
    srand((unsigned int)time(NULL));

    sa_TQRTestFifo tasks;
    saInit(&tasks, object, NUM_REQUIRES_TASKS);
    for (int i = 0; i < NUM_REQUIRES_TASKS; i++) {
        int randval      = rand() % 256 + 1;
        sum += randval;
        product = (product * randval) % 0xfffffffe + 1;
        TQRTestFifo* task = tqrtestfifoCreate(&rts, i, randval);
        ctaskRequireResource(task, trfifo);

        // start a few tasks now
        if (i < NUM_REQUIRES_TASKS / 2 || i % 3 == 0) {
            tqRun(q, &task);
        } else {
            saPushC(&tasks, object, &task);
        }
    }

    // start the rest of the tasks
    foreach(sarray, idx, TQRTestFifo*, task, tasks) {
        tqAdd(q, task);
    }
    saDestroy(&tasks);

    if (!eventWaitTimeout(&rts.notify, timeS(60)))
        ret = 1;

    if (atomicLoad(bool, &rts.fail, Relaxed) || rts.count != NUM_REQUIRES_TASKS || rts.sum != sum ||
        rts.product != product || rts.seq != NUM_REQUIRES_TASKS - 1)
        ret = 1;

    objRelease(&trfifo);
    eventDestroy(&rts.notify);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;
}

static int test_tqtest_reqlifo(void)
{
    int ret = 0;

    memset(&rts, 0, sizeof(rts));
    rts.target_count = NUM_REQUIRES_TASKS;
    rts.seq          = NUM_REQUIRES_TASKS;
    eventInit(&rts.notify);

    int sum = 0, xor = 0;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    TRLifo* trlifo = trlifoCreate();
    srand((unsigned int)time(NULL));

    sa_TQRTestLifo earlytasks;
    sa_TQRTestLifo tasks;
    saInit(&earlytasks, object, NUM_REQUIRES_TASKS);
    saInit(&tasks, object, NUM_REQUIRES_TASKS);
    for (int i = 0; i < NUM_REQUIRES_TASKS; i++) {
        int randval      = rand() % 256 + 1;
        sum += randval;
        xor ^= randval;
        TQRTestLifo* task = tqrtestlifoCreate(&rts, i, randval);
        ctaskRequireResource(task, trlifo);

        if (i % 7 == 0) {
            saPushC(&earlytasks, object, &task);
        } else {
            saPushC(&tasks, object, &task);
        }
    }

   // start a few tasks (that can't run yet) early
    foreach(sarray, idx, TQRTestLifo*, task, earlytasks) {
        tqAdd(q, task);
    }

    // start the rest of the tasks in the "correct" order
    for (int i = saSize(tasks) - 1; i >= 0; --i) {
        tqAdd(q, tasks.a[i]);
    }

    saDestroy(&earlytasks);
    saDestroy(&tasks);

    if (!eventWaitTimeout(&rts.notify, timeS(60)))
        ret = 1;

    if (atomicLoad(bool, &rts.fail, Relaxed) || rts.count != NUM_REQUIRES_TASKS || rts.sum != sum ||
        rts.xor != xor || rts.seq != 0)
        ret = 1;

    objRelease(&trlifo);
    eventDestroy(&rts.notify);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;
}

static int test_tqtest_reqgate(void)
{
    int ret = 0;

    memset(&rts2, 0, sizeof(rts2));
    rts2.target_count = NUM_REQUIRES_TASKS;
    eventInit(&rts2.notify);

    int sum = 0;

    TaskQueueConfig conf;
    tqPresetHeavy(&conf);
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    TRGate* trgate = trgateCreate(_S"Test Gate");
    srand((unsigned int)time(NULL));

    // start first half of tasks with gate closed
    for (int i = 0; i < NUM_REQUIRES_TASKS / 2; i++) {
        int randval = rand() % 256 + 1;
        sum += randval;
        TQRTestGate* task = tqrtestgateCreate(&rts2, randval);
        ctaskRequireGate(task, trgate);
        tqRun(q, &task);
    }

    osSleep(timeMS(50));
    // nothing should have gone through yet
    if (atomicLoad(int32, &rts2.count, Relaxed) > 0 || atomicLoad(int32, &rts2.sum, Relaxed) > 0)
        ret = 1;

    trgateOpen(trgate);

    // start rest of tasks with gate open
    for (int i = 0; i < NUM_REQUIRES_TASKS / 2; i++) {
        int randval = rand() % 256 + 1;
        sum += randval;
        TQRTestGate* task = tqrtestgateCreate(&rts2, randval);
        ctaskRequireGate(task, trgate);
        tqRun(q, &task);
    }

    if (!eventWaitTimeout(&rts2.notify, timeS(60)))
        ret = 1;

    if (atomicLoad(bool, &rts2.fail, Relaxed) ||
        atomicLoad(int32, &rts2.count, Relaxed) != NUM_REQUIRES_TASKS ||
        atomicLoad(int32, &rts2.sum, Relaxed) != sum)
        ret = 1;

    objRelease(&trgate);
    eventDestroy(&rts2.notify);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;
}

static int test_tqtest_manual(void)
{
    int ret   = 0;
    int total = 0, total2 = 0;
    sa_TQTest1 ttasks;

    TaskQueueConfig conf = { 0 };
    conf.flags           = TQ_Manual;
    TaskQueue* q         = tqCreate(_S"Test", &conf);
    if (!q || !tqStart(q))
        return 1;

    saInit(&ttasks, object, NUM_TASK_TEST);

    eventInit(&notifyev);

    // create tasks
    for (int i = 0; i < NUM_TASK_TEST; i++) {
        int n1 = rand(), n2 = rand();
        total += n1 + n2;
        TQTest1* t = tqtest1Create(n1, n2, &notifyev);
        if (t) {
            saPushC(&ttasks, object, &t);
        } else {
            ret = 1;
        }
    }

    // run tasks
    int ntasks = saSize(ttasks);
    for (int i = 0; i < ntasks; i++) {
        tqAdd(q, ttasks.a[i]);
    }

    // in non-oneshot mode, a single tick should process all pending tasks
    tqTick(q);

    // check tasks
    bool done = true;
    for (int i = 0; i < ntasks; i++) {
        switch (btaskState(ttasks.a[i])) {
        case TASK_Succeeded:
            total2 += ttasks.a[i]->total;
            break;
        case TASK_Failed:
            ret = 1;
            break;
        default:
            done = false;
        }
    }

    if (!done)
        ret = 1;

    eventDestroy(&notifyev);
    tqShutdown(q, timeS(60));
    tqRelease(&q);

    if (total != total2)
        ret = 1;

    saDestroy(&ttasks);

    return ret;
}

static int test_tqtest_oneshot(void)
{
    int ret   = 0;
    int total = 0, total2 = 0;
    sa_TQTest1 ttasks;

    TaskQueueConfig conf = { 0 };
    conf.flags           = TQ_Manual | TQ_Oneshot;
    TaskQueue* q         = tqCreate(_S"Test", &conf);
    if (!q || !tqStart(q))
        return 1;

    saInit(&ttasks, object, NUM_TASK_TEST);

    eventInit(&notifyev);

    // create tasks
    for (int i = 0; i < NUM_TASK_TEST; i++) {
        int n1 = rand(), n2 = rand();
        total += n1 + n2;
        TQTest1* t = tqtest1Create(n1, n2, &notifyev);
        if (t) {
            saPushC(&ttasks, object, &t);
        } else {
            ret = 1;
        }
    }

    // run tasks
    int ntasks = saSize(ttasks);
    for (int i = 0; i < ntasks; i++) {
        tqAdd(q, ttasks.a[i]);
    }

    // in oneshot mode, this should only process a single task
    tqTick(q);

    // check tasks
    int ndone = 0;
    for (int i = 0; i < ntasks; i++) {
        switch (btaskState(ttasks.a[i])) {
        case TASK_Succeeded:
            ndone++;
            break;
        case TASK_Failed:
            ndone++;
            ret = 1;
            break;
        }
    }

    // There should only be 1 task done so far
    if (ndone != 1)
        ret = 1;

    for(int i = 0; i < NUM_TASK_TEST / 2; i++) {
        tqTick(q);
    }

    // check tasks
    ndone = 0;
    for (int i = 0; i < ntasks; i++) {
        switch (btaskState(ttasks.a[i])) {
        case TASK_Succeeded:
            ndone++;
            break;
        case TASK_Failed:
            ndone++;
            ret = 1;
            break;
        }
    }

    if (ndone != NUM_TASK_TEST / 2 + 1)
        ret = 1;

    for (int i = 0; i < NUM_TASK_TEST / 2 - 1; i++) {
        tqTick(q);
    }

    eventDestroy(&notifyev);
    tqShutdown(q, timeS(60));
    tqRelease(&q);

    for (int i = 0; i < ntasks; i++) {
        if (btaskState(ttasks.a[i]) != TASK_Succeeded)
            ret = 1;
        total2 += ttasks.a[i]->total;
    }

    if (total != total2)
        ret = 1;

    saDestroy(&ttasks);

    return ret;
}

static MPTestState mps;

#define NUM_MP_TASKS 20000

static int test_tqtest_multiphase(void) {
    int ret = 0;
    int sum = 0;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    TaskQueue *q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    mps.target_count = NUM_MP_TASKS;
    eventInit(&mps.notify);
    srand((unsigned int)time(NULL));

    for (int i = 1; i <= NUM_MP_TASKS; i++) {
        int v = rand() % 9 + 1;

        TQMPTest* task = tqmptestCreate(v, i, &mps);
        switch (v) {
        case 1:
            sum += i;
            break;
        case 2:
            sum += i * 3;
            break;
        case 3:
            sum += i * 6;
            break;
        case 4:
            sum += i * 10;
            break;
        case 5:
            sum -= i * 6;
            break;
        case 6:
            sum -= i * 5;
            break;
        case 7:
            sum += i * 3;
            break;
        case 8:
            sum += i * 9;
            break;
        case 9:
            sum += i * 3;
            break;
        }
        tqRun(q, &task);
    }

    if (!eventWaitTimeout(&mps.notify, timeS(60)))
        ret = 1;

    if (atomicLoad(bool, &mps.fail, Relaxed))
        ret = 1;

    if (atomicLoad(int32, &mps.sum, Relaxed) != sum)
        ret = 1;

    eventDestroy(&mps.notify);

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    return ret;       
}

static int test_tqtest_timeout(void) {
    int ret = 0;

    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    TaskQueue* q = tqCreate(_S"Test", &conf);
    if(!q || !tqStart(q))
        return 1;

    memset(&rts, 0, sizeof(rts));
    TQTimeoutTest1* tt1 = tqtimeouttest1Create();
    TQTimeoutTest2* tt2 = tqtimeouttest2Create(&rts);
    tt2->flags |= TASK_Cancel_Expired;
    ctaskDependOnTimeout(tt2, tt1, timeMS(250));

    tqAdd(q, tt2);
    tqAdd(q, tt1);

    if (!taskWait(tt2, timeS(5)))
        ret = 1;

    if (!taskIsComplete(tt2) || taskSucceeded(tt2))
        ret = 1;

    if (rts.count != 0)
        ret = 1;

    tqShutdown(q, timeS(60));
    tqRelease(&q);

    objRelease(&tt1);
    objRelease(&tt2);

    return ret;
}

testfunc tqtest_funcs[] = {
    {"task",                   test_tqtest_task                 },
    { "failure",               test_tqtest_failure              },
    { "concurrency_inworker",  test_tqtest_concurrency_inworker },
    { "concurrency_dedicated", test_tqtest_concurrency_dedicated},
    { "call",                  test_tqtest_call                 },
    { "sched",                 test_tqtest_sched                },
    { "monitor_inworker",      test_tqtest_monitor_inworker     },
    { "monitor_dedicated",     test_tqtest_monitor_dedicated    },
    { "depend",                test_tqtest_depend               },
    { "reqmutex",              test_tqtest_reqmutex             },
    { "reqfifo",               test_tqtest_reqfifo              },
    { "reqlifo",               test_tqtest_reqlifo              },
    { "reqgate",               test_tqtest_reqgate              },
    { "timeout",               test_tqtest_timeout              },
    { "manual",                test_tqtest_manual               },
    { "oneshot",               test_tqtest_oneshot              },
    { "multiphase",            test_tqtest_multiphase           },
    { 0,                       0                                }
};
