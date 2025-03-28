#pragma once

#include <cx/cx.h>
#include <cx/platform/base.h>

#include <cx/thread/threadobj.h>

enum ThreadPriority {
    THREAD_Normal = 0,
    THREAD_Batch,
    THREAD_Low,
    THREAD_Idle,
    THREAD_High,
    THREAD_Higher,
    THREAD_Realtime
};

// defined in platform-specific thread module
_Ret_opt_valid_
Thread *thrCurrent(void);

intptr thrOSThreadID(_In_ Thread *thread);
intptr thrCurrentOSThreadID(void);      // works even on non-cx threads

_Ret_opt_valid_ _Check_return_
Thread *_thrCreate(_In_ threadFunc func, _In_ strref name, int n, _In_ stvar args[], bool ui);
// Thread* thrCreate(threadFunc func, strref name, ...)
//
// Creates and starts a thread. The return value MUST be released with thrRelease to avoid leaking memory.
#define thrCreate(func, name, ...) _thrCreate(func, name, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ }, false)
#define thrCreateUI(func, name, ...) _thrCreate(func, name, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ }, true)

void _thrRun(_In_ threadFunc func, _In_ strref name, int n, _In_ stvar args[]);
// bool thrRun(threadFunc func, strref name, ...)
//
// Creates and starts a thread.
#define thrRun(func, name, ...) _thrRun(func, name, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

_meta_inline bool thrRunning(_In_ Thread *thread)
{
    return atomicLoad(bool, &thread->running, Acquire);
}

// for use in a thread loop, returns false if the thread should exit
_meta_inline bool thrLoop(_In_ Thread *thread)
{
    return !atomicLoad(bool, &thread->requestExit, Acquire);
}

bool thrRequestExit(_In_ Thread *thread);
bool thrWait(_In_ Thread *thread, int64 timeout);
bool thrShutdown(_In_ Thread *thread);
int thrShutdownMany(_In_ sa_Thread threads);
bool _thrPlatformSetPriority(_Inout_ Thread *thread, int prio);
#define thrSetPriority(thread, prio) _thrPlatformSetPriority(thread, THREAD_##prio)
#define thrSetPriorityV(thread, prio) _thrPlatformSetPriority(thread, prio)

#define thrRelease(pthread) objRelease(pthread)

typedef struct Event Event;
// Registers a thread as a system thread. System threads are background threads that run independently
// of the main program and are typically library-created. They are notified at program exit and given
// an opportunity to perform cleanup before being destroyed.
void thrRegisterSysThread(_Inout_ Thread *thread);
