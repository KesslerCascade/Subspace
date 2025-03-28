#include <cx/thread.h>
#include <cx/string/strtest.h>
#include <cx/container.h>
#include <cx/thread/condvar.h>
#include <cx/thread/futex.h>
#include <cx/thread/event.h>
#include <cx/thread/mutex.h>
#include <cx/thread/rwlock.h>
#include <cx/thread/sema.h>
#include <cx/platform/os.h>

#define TEST_FILE thrtest
#define TEST_FUNCS thrtest_funcs
#include "common.h"

int thrtest1[16] = {0};
Thread *thrtest2[16] = {0};

static int thrproc1(Thread *self)
{
    int32 slot, count;

    if (!(stvlNext(&self->args, int32, &slot) &&
          stvlNext(&self->args, int32, &count)))
        return 0;

    thrtest2[slot] = thrCurrent();

    for (int i = 0; i < count; i++) {
        thrtest1[slot]++;
    }

    return 0;
}

#define BASIC_THREADS 16
static int test_basic()
{
    Thread *threads[BASIC_THREADS];
    int ret = 0;
    int i;

    for (i = 0; i < BASIC_THREADS; i++) {
        threads[i] = thrCreate(thrproc1, _S"Basic Test Thread", stvar(int32, i), stvar(int32, 1000000 + i * 100000));
        if (!threads[i])
            return 1;
        thrSetPriorityV(threads[i], i % (THREAD_Realtime + 1));
    }

    for (i = 0; i < BASIC_THREADS; i++) {
        thrWait(threads[i], timeForever);
        thrShutdown(threads[i]);
        thrRelease(&threads[i]);
        if (thrtest1[i] != 1000000 + i * 100000)
            ret = 1;
    }

    return ret;
}

static Futex testftx;

static int thrproc2(Thread *self)
{
    bool dec;
    int32 count;

    if (!(stvlNext(&self->args, uint8, &dec) &&
          stvlNext(&self->args, int32, &count)))
        return 0;

    for (int i = 0; i < count; i++) {
        if (dec) {
            int32 val = atomicLoad(int32, &testftx.val, Relaxed);
            while (val == 0 || !atomicCompareExchange(int32, weak, &testftx.val, &val, val - 1, Acquire, Relaxed)) {
                if (val == 0) {
                    futexWait(&testftx, 0, timeForever);
                } else
                    osYield();
                val = atomicLoad(int32, &testftx.val, Relaxed);
            }
        }
        else {
            atomicFetchAdd(int32, &testftx.val, 1, Relaxed);
            futexWake(&testftx);
        }
    }

    return 0;
}

#define FUTEX_PRODUCERS 2
#define FUTEX_CONSUMERS 8
#define FUTEX_COUNT 524288
static int test_futex()
{
    int ret = 0;
    futexInit(&testftx, 0);

    int i;
    Thread *producers[FUTEX_PRODUCERS];
    Thread *consumers[FUTEX_CONSUMERS];
    for (i = 0; i < FUTEX_CONSUMERS; i++) {
        consumers[i] = thrCreate(thrproc2, _S"Futex Consumer", stvar(uint8, 1), stvar(int32, FUTEX_COUNT / FUTEX_CONSUMERS));
        if (!consumers[i])
            return 1;
    }
    for (i = 0; i < FUTEX_PRODUCERS; i++) {
        producers[i] = thrCreate(thrproc2, _S"Futex Producer", stvar(uint8, 0), stvar(int32, FUTEX_COUNT / FUTEX_PRODUCERS));
        if (!producers[i])
            return 1;
    }

    for (i = 0; i < FUTEX_PRODUCERS; i++) {
        thrWait(producers[i], timeForever);
        thrShutdown(producers[i]);
        thrRelease(&producers[i]);
    }
    for (i = 0; i < FUTEX_CONSUMERS; i++) {
        thrWait(consumers[i], timeForever);
        thrShutdown(consumers[i]);
        thrRelease(&consumers[i]);
    }

    if (atomicLoad(int32, &testftx.val, Acquire) != 0)
        ret = 1;

    return ret;
}

static Semaphore testsem;

static int thrproc2s(Thread *self)
{
    bool dec;
    int32 count;

    if (!(stvlNext(&self->args, uint8, &dec) &&
          stvlNext(&self->args, int32, &count)))
        return 0;

    for (int i = 0; i < count; i++) {
        if (dec)
            semaDec(&testsem);
        else
            semaInc(&testsem, 1);
    }

    return 0;
}

#define SEMA_PRODUCERS 2
#define SEMA_CONSUMERS 8
#define SEMA_COUNT 524288
static int test_sema()
{
    int ret = 0;
    semaInit(&testsem, 0);

    int i;
    Thread *producers[SEMA_PRODUCERS] = { 0 };
    Thread *consumers[SEMA_CONSUMERS] = { 0 };
    for (i = 0; i < SEMA_CONSUMERS; i++) {
        consumers[i] = thrCreate(thrproc2s, _S"Semaphore Consumer", stvar(uint8, 1), stvar(int32, SEMA_COUNT / SEMA_CONSUMERS));
        if (!consumers[i])
            return 1;
    }
    for (i = 0; i < SEMA_PRODUCERS; i++) {
        producers[i] = thrCreate(thrproc2s, _S"Semaphore Producer", stvar(uint8, 0), stvar(int32, SEMA_COUNT / SEMA_PRODUCERS));
        if (!producers[i])
            return 1;
    }

    for (i = 0; i < SEMA_PRODUCERS; i++) {
        thrWait(producers[i], timeForever);
        thrShutdown(producers[i]);
        thrRelease(&producers[i]);
    }
    for (i = 0; i < SEMA_CONSUMERS; i++) {
        thrWait(consumers[i], timeForever);
        thrShutdown(consumers[i]);
        thrRelease(&consumers[i]);
    }

    if (atomicLoad(int32, &testsem.ftx.val, Acquire) != 0)
        ret = 1;

    if (!semaDestroy(&testsem))
        ret = 1;

    return ret;
}

static atomic(bool) fail;
static int64 testint1;
static int64 testint2;
static int64 testint3;
static Mutex testmtx;

static int thrproc3(Thread *self)
{
    int32 count;
    if (!stvlNext(&self->args, int32, &count))
        return 0;

    for (int i = 0; i < count; i++) {
        mutexAcquire(&testmtx);
        testint1++;
        testint2++;
        testint3++;
        mutexRelease(&testmtx);

        withMutex(&testmtx) {
            if (testint1 != testint2 || testint2 != testint3)
                atomicStore(bool, &fail, true, Release);
        }
    }

    return 0;
}

#define MTX_THREADS 32
#define MTX_COUNT 1048576
static int test_mutex()
{
    atomicStore(bool, &fail, false, Release);
    testint1 = 0;
    testint2 = 0;
    testint3 = 0;
    mutexInit(&testmtx);

    int i;
    Thread *threads[MTX_THREADS];

    for (i = 0; i < MTX_THREADS; i++) {
        threads[i] = thrCreate(thrproc3, _S"Mutex Test", stvar(int32, MTX_COUNT / MTX_THREADS));
        if (!threads[i])
            return 1;
    }

    for (i = 0; i < MTX_THREADS; i++) {
        thrWait(threads[i], timeForever);
        thrShutdown(threads[i]);
        thrRelease(&threads[i]);
    }

    int ret = atomicLoad(bool, &fail, Acquire) ? 1 : 0;
    if (testint1 != MTX_COUNT || testint2 != MTX_COUNT || testint3 != MTX_COUNT)
        ret = 1;

    mutexDestroy(&testmtx);

    return ret;
}

static RWLock testrw;
static atomic(bool) rthread_exit = atomicInit(false);

static int thrproc4r(Thread *self)
{
    while (!atomicLoad(bool, &rthread_exit, Acquire)) {
        withReadLock(&testrw) {
            for (int i = 0; i < 16; i++) {
                if (testint1 != testint2 || testint2 != testint3)
                    atomicStore(bool, &fail, true, Release);

            }
        }
        osYield();
    }

    return 0;
}

static int thrproc4w(Thread *self)
{
    int32 count;
    if (!stvlNext(&self->args, int32, &count))
        return 0;

    for (int i = 0; i < count; i++) {
        rwlockAcquireWrite(&testrw);
        testint1++;
        testint2++;
        testint3++;
        if (i % 16 == 0) {
            // every so often try a downgrade and make sure read consistency is good
            rwlockDowngradeWrite(&testrw);
            for (int j = 0; j < 16; j++) {
                if (testint1 != testint2 || testint2 != testint3)
                    atomicStore(bool, &fail, true, Release);
            }
            rwlockReleaseRead(&testrw);
        } else {
            rwlockReleaseWrite(&testrw);
        }
    }

    return 0;
}

#define RW_WTHREADS 4
#define RW_RTHREADS 16
#define RW_COUNT 32768
static int test_rwlock()
{
    atomicStore(bool, &fail, false, Release);
    testint1 = 0;
    testint2 = 0;
    testint3 = 0;
    rwlockInit(&testrw);

    int i;
    Thread *rthreads[RW_RTHREADS];
    Thread *wthreads[RW_WTHREADS];

    for (i = 0; i < RW_RTHREADS; i++) {
        rthreads[i] = thrCreate(thrproc4r, _S"Reader Thread", stvar(int32, 0));
        if (!rthreads[i])
            return 1;
    }
    for (i = 0; i < RW_WTHREADS; i++) {
        wthreads[i] = thrCreate(thrproc4w, _S"Writer Thread", stvar(int32, RW_COUNT / RW_WTHREADS));
        if (!wthreads[i])
            return 1;
    }

    for (i = 0; i < RW_WTHREADS; i++) {
        thrWait(wthreads[i], timeForever);
        thrShutdown(wthreads[i]);
        thrRelease(&wthreads[i]);
    }
    atomicStore(bool, &rthread_exit, true, Release);
    for (i = 0; i < RW_RTHREADS; i++) {
        thrWait(rthreads[i], timeForever);
        thrShutdown(rthreads[i]);
        thrRelease(&rthreads[i]);
    }

    rwlockDestroy(&testrw);

    int ret = atomicLoad(bool, &fail, Acquire) ? 1 : 0;
    if (testint1 != RW_COUNT || testint2 != RW_COUNT || testint3 != RW_COUNT)
        ret = 1;

    return ret;
}

static Event testev;

#define EVENT_CONSUMERS 4
#define EVENT_PRODUCERS 4
#define EVENT_COUNT 32768

static int32 evthrcount[EVENT_CONSUMERS];
static atomic(int32) evsignaled;
static atomic(int32) evwork;
static atomic(int32) evdone;

static int thrproc5c(Thread *self)
{
    int thrid;
    if (!stvlNext(&self->args, int32, &thrid))
        return 0;

    int32 work;
    for (;;) {
        eventWait(&testev);

        if (atomicLoad(bool, &rthread_exit, Acquire))
            break;

        int32 sig = atomicLoad(int32, &evsignaled, Relaxed);
        // we shouldn't wake up if the event hasn't been signaled
        if (sig == 0)
            atomicStore(bool, &fail, true, Relaxed);
        else
            atomicFetchSub(int32, &evsignaled, 1, Relaxed);

        work = atomicLoad(int32, &evwork, Relaxed);
        do {
            if (work > 0 && atomicCompareExchange(int32, strong, &evwork, &work, work - 1, Acquire, Acquire)) {
                evthrcount[thrid]++;
                atomicFetchAdd(int32, &evdone, 1, Release);
            }
            work = atomicLoad(int32, &evwork, Relaxed);
        } while (work > 0);
    }

    return 0;
}

static int thrproc5p(Thread *self)
{
    int count;
    if (!stvlNext(&self->args, int32, &count))
        return 0;

    for (int i = 0; i < count; i++) {
        atomicFetchAdd(int32, &evwork, 1, Acquire);
        atomicFetchAdd(int32, &evsignaled, 1, Acquire);
        while (!eventSignal(&testev)) {
            osYield();
        }
    }

    return 0;
}

static int test_event_sub(bool spin)
{
    atomicStore(bool, &rthread_exit, false, Release);
    atomicStore(bool, &fail, false, Release);
    atomicStore(int32, &evwork, 0, Release);
    atomicStore(int32, &evdone, 0, Release);
    memset(evthrcount, 0, sizeof(evthrcount));

    if (spin)
        eventInit(&testev, EV_Spin);
    else
        eventInit(&testev);

    int i;
    Thread *cthreads[EVENT_CONSUMERS];
    Thread *pthreads[EVENT_PRODUCERS];

    for (i = 0; i < EVENT_CONSUMERS; i++) {
        cthreads[i] = thrCreate(thrproc5c, _S"Event Consumer", stvar(int32, i));
        if (!cthreads[i])
            return 1;
    }
    for (i = 0; i < EVENT_PRODUCERS; i++) {
        pthreads[i] = thrCreate(thrproc5p, _S"Event Producer", stvar(int32, EVENT_COUNT / EVENT_PRODUCERS));
        if (!pthreads[i])
            return 1;
    }

    for (i = 0; i < EVENT_PRODUCERS; i++) {
        thrWait(pthreads[i], timeForever);
        thrShutdown(pthreads[i]);
        thrRelease(&pthreads[i]);
    }

    while (atomicLoad(int32, &evwork, Relaxed) > 0) {
        osYield();
    }

    atomicStore(bool, &rthread_exit, true, Release);
    eventSignalLock(&testev);
    for (i = 0; i < EVENT_CONSUMERS; i++) {
        thrWait(cthreads[i], timeForever);
        thrShutdown(cthreads[i]);
        thrRelease(&cthreads[i]);
    }

    eventDestroy(&testev);

    int tcount = 0;
    for (int i = 0; i < EVENT_CONSUMERS; i++) {
        tcount += evthrcount[i];
    }

    if (atomicLoad(bool, &fail, Acquire))
        return 1;

    if (atomicLoad(int32, &evsignaled, Acquire) != 0)
        return 1;

    if (atomicLoad(int32, &evdone, Acquire) != EVENT_COUNT)
        return 1;

    if (tcount != EVENT_COUNT)
        return 1;

    return 0;
}

static int test_event()
{
    return test_event_sub(false);
}

static int test_event_s()
{
    return test_event_sub(true);
}

static int thrproc6(Thread *self)
{
    // first test should take less than 100ms
    if (futexWait(&testftx, 0, timeFromMsec(100)) != FUTEX_Waited)
        atomicStore(bool, &fail, true, Release);

    atomicFetchSub(int32, &testftx.val, 1, Relaxed);

    // second test should take more than 100ms
    if (futexWait(&testftx, 0, timeFromMsec(100)) != FUTEX_Timeout)
        atomicStore(bool, &fail, true, Release);

    atomicFetchSub(int32, &testftx.val, 1, Relaxed);

    return 0;
}

static int test_timeout()
{
    atomicStore(bool, &fail, false, Release);
    futexInit(&testftx, 0);

    Thread *testthr = thrCreate(thrproc6, _S"Timeout Test", stvNone);
    if (!testthr)
        return 1;

    osSleep(timeFromMsec(50));
    atomicFetchAdd(int32, &testftx.val, 1, Relaxed);
    futexWake(&testftx);

    osSleep(timeFromMsec(150));
    atomicFetchAdd(int32, &testftx.val, 1, Relaxed);
    futexWake(&testftx);

    thrWait(testthr, timeForever);
    thrShutdown(testthr);
    thrRelease(&testthr);

    int ret = atomicLoad(bool, &fail, Acquire) ? 1 : 0;
    return ret;
}

#define CV_CONSUMERS 4
#define CV_PRODUCERS 4
#define CV_COUNT 32768

static Mutex cvmtx;
static CondVar dataneeded;
static CondVar dataready;
static int cvready;
static int cvin;
static int cvout;
static int cvproduced[CV_PRODUCERS];
static int cvconsumed[CV_CONSUMERS];

static int thrproc7p(Thread *self)
{
    int slot, count;
    if (!stvlNext(&self->args, int32, &slot))
        return 0;
    if (!stvlNext(&self->args, int32, &count))
        return 0;

    for (int i = 0; i < count; i++) {
        mutexAcquire(&cvmtx);
        while (cvready == 1)
            cvarWait(&dataneeded, &cvmtx);
        cvin++;
        cvproduced[slot]++;
        cvready = 1;
        cvarSignal(&dataready);
        mutexRelease(&cvmtx);
    }

    return 0;
}

static int thrproc7c(Thread *self)
{
    int slot, count;
    if (!stvlNext(&self->args, int32, &slot))
        return 0;
    if (!stvlNext(&self->args, int32, &count))
        return 0;

    for (int i = 0; i < count; i++) {
        mutexAcquire(&cvmtx);
        while (cvready == 0)
            cvarWait(&dataready, &cvmtx);
        cvin--;
        cvout++;
        cvconsumed[slot]++;
        cvready = 0;
        cvarSignal(&dataneeded);
        mutexRelease(&cvmtx);
    }

    return 0;
}

static int test_condvar()
{
    mutexInit(&cvmtx);
    cvarInit(&dataneeded);
    cvarInit(&dataready);

    int i;
    Thread *cthreads[CV_CONSUMERS];
    Thread *pthreads[CV_PRODUCERS];

    for (i = 0; i < CV_CONSUMERS; i++) {
        cthreads[i] = thrCreate(thrproc7c, _S"Condition Variable Consumer", stvar(int32, i), stvar(int32, CV_COUNT / CV_CONSUMERS));
        if (!cthreads[i])
            return 1;
    }
    for (i = 0; i < CV_PRODUCERS; i++) {
        pthreads[i] = thrCreate(thrproc7p, _S"Condition Variable Producer", stvar(int32, i), stvar(int32, CV_COUNT / CV_PRODUCERS));
        if (!pthreads[i])
            return 1;
    }

    for (i = 0; i < CV_PRODUCERS; i++) {
        thrWait(pthreads[i], timeForever);
        thrShutdown(pthreads[i]);
        thrRelease(&pthreads[i]);
    }

    for (i = 0; i < CV_CONSUMERS; i++) {
        thrWait(cthreads[i], timeForever);
        thrShutdown(cthreads[i]);
        thrRelease(&cthreads[i]);
    }

    cvarDestroy(&dataneeded);
    cvarDestroy(&dataready);
    mutexDestroy(&cvmtx);

    if (cvin != 0 || cvout != CV_COUNT || cvready != 0)
        return 1;

    int tcount = 0;
    for (int i = 0; i < CV_CONSUMERS; i++) {
        tcount += cvconsumed[i];
    }
    if (tcount != CV_COUNT)
        return 1;

    tcount = 0;
    for (int i = 0; i < CV_PRODUCERS; i++) {
        tcount += cvproduced[i];
    }
    if (tcount != CV_COUNT)
        return 1;

    return 0;
}

testfunc thrtest_funcs[] = {
    { "basic", test_basic },
    { "futex", test_futex },
    { "sema", test_sema },
    { "mutex", test_mutex },
    { "rwlock", test_rwlock },
    { "event", test_event },
    { "event_s", test_event_s },
    { "timeout", test_timeout },
    { "condvar", test_condvar },
    { 0, 0 }
};
