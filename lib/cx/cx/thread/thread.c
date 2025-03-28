#include "thread_private.h"
#include <cx/container/foreach.h>
#include <cx/container/sarray.h>
#include <cx/time/time.h>
#include <cx/string.h>

#define THREAD_SHUTDOWN_TIMEOUT timeFromSeconds(30)

_Use_decl_annotations_
Thread* _thrCreate(threadFunc func, strref name, int n, stvar args[], bool ui)
{
    Thread *ret = _throbjCreate(func, name, n, args, ui);

    atomicStore(bool, &ret->running, true, Relaxed);
    // This is the reference for the newly created thread, it gets released inside the
    // platform-specific thread proc just before exiting
    Thread *cret = objAcquire(ret);
    if (!_thrPlatformStart(ret)) {
        // if thread creation failed, have to release both the thread's reference
        // AND the one we'd return to the caller
        objRelease(&cret);
        objRelease(&ret);
        return NULL;
    }

    return ret;
}

_Use_decl_annotations_
void _thrRun(threadFunc func, strref name, int n, stvar args[])
{
    Thread *ret = _throbjCreate(func, name, n, args, false);

    atomicStore(bool, &ret->running, true, Relaxed);
    if (!_thrPlatformStart(ret)) {
        objRelease(&ret);
        relFatalError("Failed to start thread");
    }
}

_Use_decl_annotations_
bool thrWait(Thread *thread, int64 timeout)
{
    if (!atomicLoad(bool, &thread->running, Acquire))
        return true;

    bool ret = _thrPlatformWait(thread, timeout);

    if (ret) {
        devAssert(!atomicLoad(bool, &thread->running, Acquire));
    }

    return ret;
}

_Use_decl_annotations_
bool thrShutdown(Thread *thread)
{
    if (!thread)
        return false;

    if (atomicLoad(bool, &thread->running, Acquire)) {
        thrRequestExit(thread);
    }

    // UNIX pthreads needs to clean up exited threads by joining (waiting on) them
    bool ret = _thrPlatformWait(thread, THREAD_SHUTDOWN_TIMEOUT);

    if (ret) {
        devAssert(!atomicLoad(bool, &thread->running, Acquire));
    }

    return ret;
}

_Use_decl_annotations_
int thrShutdownMany(sa_Thread threads)
{
    int64 start = clockTimer();

    // pass 1: signal all of the threads to exit
    foreach(sarray, idx, Thread *, thread, threads) {
        if (atomicLoad(bool, &thread->running, Acquire)) {
            thrRequestExit(thread);
        }
    }

    int count = 0;
    // pass 2: wait for them to finish exiting
    foreach(sarray, idx, Thread *, thread, threads)
    {
        // Since all threads are shutting down in parallel, don't wait
        // THREAD_SHUTDOWN_TIMEOUT on each one separately. Instead base
        // the timeout on the total time this function has been waiting
        // for any thread.
        int64 wait = max(0, THREAD_SHUTDOWN_TIMEOUT - (clockTimer() - start));
        if (_thrPlatformWait(thread, wait))
            count++;
    }

    return count;
}

_Use_decl_annotations_
bool thrRequestExit(Thread *thread)
{
    if (!thread || !atomicLoad(bool, &thread->running, Acquire))
        return false;

    atomicStore(bool, &thread->requestExit, true, Release);
    eventSignalLock(&thread->notify);
    return true;
}
