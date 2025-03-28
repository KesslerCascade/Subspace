#include <cx/thread/thread_private.h>
#include <cx/platform/win.h>
#include <cx/string.h>
#include <cx/time/time.h>
#include <cx/utils/lazyinit.h>
#include <process.h>
#include "win_thread_threadobj.h"

typedef HRESULT (WINAPI *SetThreadDescription_t)(HANDLE hThread, PCWSTR lpThreadDescription);
static SetThreadDescription_t pSetThreadDescription;

static WinThread *mainthread;
static _Thread_local WinThread *curthread;

static LazyInitState platformThreadInitState;
static void platformThreadInit(void *dummy)
{
    // synthesize a Thread structure for the main thread, so thrCurrent can work

    // We assume that the first thread that creates another thread is the main thread.
    // This may not be a correct assumption, but is the best we can do consistently
    // across all platforms without shenanigans.

    mainthread = _winthrobjCreate();
    if (mainthread) {
        mainthread->handle = GetCurrentThread();
        mainthread->id = GetCurrentThreadId();
        strDup(&mainthread->name, _S"Main");
    } else {
        relFatalError("Failed to create main thread");
    }

    atomicStore(bool, &mainthread->running, true, Relaxed);

    curthread = mainthread;

    // See if the SetThreadDescription API is available for setting thread names
    // on Windows 10 and later
    HANDLE hDll = LoadLibrary(TEXT("kernel32.dll"));
    relAssertMsg(hDll, "Failed to load kernel32.dll");
    if (hDll)
        pSetThreadDescription = (SetThreadDescription_t)GetProcAddress(hDll, "SetThreadDescription");
}

static unsigned __stdcall _thrEntryPoint(void *data)
{
    WinThread *thr = (WinThread*)data;
    if (!thr)
        goto out;

    thr->id = GetCurrentThreadId();
    curthread = thr;

    if (pSetThreadDescription)
        pSetThreadDescription(thr->handle, strToUTF16S(thr->name));

    thr->exitCode = thr->entry(Thread(thr));
    saClear(&thr->_argsa);
    thr->args.count = 0;

    atomicStore(bool, &thr->running, false, Release);
    objRelease(&thr);

out:
    _endthreadex(0);
    return 0;
}

_Use_decl_annotations_
Thread *_thrPlatformCreate() {
    WinThread *wthr = _winthrobjCreate();
    return Thread(wthr);
}

bool _thrPlatformStart(Thread *thread)
{
    lazyInit(&platformThreadInitState, &platformThreadInit, NULL);

    WinThread *thr = objDynCast(WinThread, thread);

    if (!thr || thr->handle)
        return false;

    thr->handle = (HANDLE)_beginthreadex(NULL, 0, _thrEntryPoint, thr, 0, NULL);
    return !!thr->handle;
}

bool _thrPlatformWait(Thread *thread, int64 timeout)
{
    WinThread *thr = objDynCast(WinThread, thread);
    if (!thr) return false;

    return WaitForSingleObject(thr->handle, (timeout == timeForever) ? INFINITE : (DWORD)timeToMsec(timeout)) == WAIT_OBJECT_0;
}

_Use_decl_annotations_
bool _thrPlatformSetPriority(Thread *thread, int prio)
{
    WinThread *thr = objDynCast(WinThread, thread);
    if (!thr) return false;

    if (prio == THREAD_Batch) {
        // background mode only works on the current thread, so also adjust priority below
        SetThreadPriority(thr->handle, THREAD_MODE_BACKGROUND_BEGIN);
        thr->background = true;
    } else if (thr->background && prio) {
        SetThreadPriority(thr->handle, THREAD_MODE_BACKGROUND_END);
        thr->background = false;
    }

    int winprio = THREAD_PRIORITY_NORMAL;
    switch (prio) {
    case THREAD_Normal:
        winprio = THREAD_PRIORITY_NORMAL;
        break;
    case THREAD_Batch:
        winprio = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    case THREAD_Low:
        winprio = THREAD_PRIORITY_LOWEST;
        break;
    case THREAD_Idle:
        winprio = THREAD_PRIORITY_IDLE;
        break;
    case THREAD_High:
        winprio = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    case THREAD_Higher:
        winprio = THREAD_PRIORITY_HIGHEST;
        break;
    case THREAD_Realtime:
        winprio = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    }

    return SetThreadPriority(thr->handle, winprio);
}

_Use_decl_annotations_
Thread *thrCurrent(void)
{
    return Thread(curthread);
}

_Use_decl_annotations_
intptr thrOSThreadID(Thread *thread)
{
    WinThread *thr = objDynCast(WinThread, thread);
    if (!thr) return 0;

    return thr->id;
}

intptr thrCurrentOSThreadID(void)
{
    return GetCurrentThreadId();
}
