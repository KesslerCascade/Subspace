#include <cx/thread.h>
#include <cx/string.h>
#include <cx/utils/lazyinit.h>
#include <unistd.h>
#define _GNU_SOURCE
#define __USE_GNU
#include <pthread.h>
#include <sched.h>
#include "wasm_thread_threadobj.h"

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

    curthread = thr;

    pthread_cleanup_push(_thrCancelCleanup, thr);
    thr->exitCode = thr->entry(Thread(thr));
    saClear(&thr->_argsa);
    thr->args.count = 0;
    pthread_cleanup_pop(true);
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

// WebAssembly doesn't support thread priority at all
bool _thrPlatformSetPriority(Thread *thread, int prio) {
    return false;
}

Thread *thrCurrent(void)
{
    return Thread(curthread);
}

// WebAssembly doesn't have OS-visible thread IDs to speak of, so fake it
// by just using the value of the thread pointer.

intptr thrOSThreadID(Thread *thread)
{
    return (intptr)thread;
}

intptr thrCurrentOSThreadID(void)
{
    return (intptr)curthread;
}
