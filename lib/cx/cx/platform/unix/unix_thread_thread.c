#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include <cx/thread.h>
#include <cx/string.h>
#include <cx/utils/lazyinit.h>
#include <unistd.h>
#if defined(_PLATFORM_FBSD)
#include <pthread.h>
#include <pthread_np.h>
#include <sys/thr.h>
#elif defined(_PLATFORM_LINUX)
#define _GNU_SOURCE
#define __USE_GNU
#include <pthread.h>
#include <sys/syscall.h>
#endif
#include <sched.h>
#include "unix_thread_threadobj.h"

static int getThreadId()
{
#if defined(_PLATFORM_LINUX)
    return syscall(SYS_gettid);
#elif defined(_PLATFORM_FBSD)
    long tid;
    thr_self(&tid);
    return (int)tid;
#else
    return getpid();
#endif
}

static UnixThread *mainthread;
static _Thread_local UnixThread *curthread;

static LazyInitState platformThreadInitState;
static void platformThreadInit(void *dummy)
{
    // synthesize a Thread structure for the main thread, so thrCurrent can work

    // We assume that the first thread that creates another thread is the main thread.
    // This may not be a correct assumption, but is the best we can do consistently
    // across all platforms without shenanigans.

    mainthread = _unixthrobjCreate();

    mainthread->pthr = pthread_self();
    mainthread->id = getThreadId();
    strDup(&mainthread->name, _S"Main");
    atomicStore(bool, &mainthread->running, true, Relaxed);

    curthread = mainthread;
}

static void _thrCancelCleanup(void *data)
{
    UnixThread *thr = (UnixThread*)data;
    atomicStore(bool, &thr->running, false, Release);
}

static void* _thrEntryPoint(void *data)
{
    UnixThread *thr = (UnixThread*)data;
    if (!thr)
        goto out;

    thr->id = getThreadId();
    curthread = thr;

    pthread_cleanup_push(_thrCancelCleanup, thr);
    pthread_setname_np(thr->pthr, strC(thr->name));

    thr->exitCode = thr->entry(Thread(thr));
    saClear(&thr->_argsa);
    thr->args.count = 0;

    pthread_cleanup_pop(true);              // calls _thrCancelCleanup
    objRelease(&thr);

out:
    pthread_exit(NULL);
    return NULL;
}

Thread *_thrPlatformCreate() {
    UnixThread *uthr = _unixthrobjCreate();
    return Thread(uthr);
}

bool _thrPlatformStart(Thread *thread)
{
    lazyInit(&platformThreadInitState, &platformThreadInit, NULL);

    UnixThread *thr = objDynCast(UnixThread, thread);

    if (!thr || thr->pthr)
        return 0;

    bool ret = !pthread_create(&thr->pthr, NULL, _thrEntryPoint, thr);
    // pthreads has annoying limitations so it may be necessary to adjust
    // thready priority even for "Normal" threads as a result...
    if (ret)
        _thrPlatformSetPriority(thread, THREAD_Normal);
    return ret;
}

bool _thrPlatformWait(Thread *thread, int64 timeout)
{
    UnixThread *thr = objDynCast(UnixThread, thread);
    if (!thr) return false;

    // some pthreads implementations will crash if you try to join
    // a thread that's already been joined
    if (thr->joined)
        return true;

    if (timeout == timeForever)
        thr->joined = !pthread_join(thr->pthr, NULL);
    else {
        struct timespec ts;
	void *unused;
        timeToAbsTimespec(&ts, clockWall() + timeout);
        thr->joined = !pthread_timedjoin_np(thr->pthr, &unused, &ts);
    }
    return thr->joined;
}

#ifdef _PLATFORM_LINUX
// Linux pthreads sucks because it doesn't have any higher-than-normal
// priority levels other than realtime. It can do per-thread nice (maybe,
// seems to depend on kernel version and particular implementation) which
// would work except that there isn't a way to set it for anything other
// than the current thread. That makes things awkward and doesn't fit well
// with our API. For now we just don't support THREAD_High or Higher.
bool _thrPlatformSetPriority(Thread *thread, int prio) {
    UnixThread *thr = objDynCast(UnixThread, thread);
    if (!thr) return false;

    struct sched_param param = {0};
    int policy = SCHED_OTHER;

    switch(prio) {
        case THREAD_Normal:
            policy = SCHED_OTHER;
            break;
        case THREAD_Batch:
            policy = SCHED_BATCH;
            break;
        case THREAD_Low:
            policy = SCHED_IDLE;
            break;
        case THREAD_Idle:
            policy = SCHED_IDLE;
            break;
        case THREAD_High:
            policy = SCHED_OTHER;
            break;
        case THREAD_Higher:
            policy = SCHED_OTHER;
            break;
        case THREAD_Realtime:
            policy = SCHED_RR;
            param.sched_priority = 1;
            break;
    }

    return !pthread_setschedparam(thr->pthr, policy, &param);
}
#endif

#ifdef _PLATFORM_FBSD
// FreeBSD pthreads sucks because it doesn't have any *lower*-than-normal
// priority levels. It does support priority levels on SCHED_OTHER though,
// so we can sort of fake it with that.
bool _thrPlatformSetPriority(Thread *thread, int prio) {
    UnixThread *thr = objDynCast(UnixThread, thread);
    if (!thr) return false;

    struct sched_param param = {0};
    int policy = SCHED_OTHER;
    int maxprio = sched_get_priority_max(SCHED_OTHER);

    switch(prio) {
        case THREAD_Normal:
            param.sched_priority = maxprio / 2;
            break;
        case THREAD_Batch:
            param.sched_priority = maxprio / 2 - 1;
            break;
        case THREAD_Low:
            param.sched_priority = maxprio / 4;
            break;
        case THREAD_Idle:
            param.sched_priority = sched_get_priority_min(SCHED_OTHER);
            break;
        case THREAD_High:
            param.sched_priority = maxprio / 4 * 3;
            break;
        case THREAD_Higher:
            param.sched_priority = maxprio;
            break;
        case THREAD_Realtime:
            policy = SCHED_RR;
            param.sched_priority = 1;
            break;
    }

    return !pthread_setschedparam(thr->pthr, policy, &param);
}
#endif

Thread *thrCurrent(void)
{
    return Thread(curthread);
}

intptr thrOSThreadID(Thread *thread)
{
    UnixThread *thr = objDynCast(UnixThread, thread);
    if (!thr) return 0;

    return thr->id;
}

intptr thrCurrentOSThreadID(void)
{
    return getThreadId();
}
