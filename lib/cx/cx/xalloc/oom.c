#include "xalloc_private.h"

#include <cx/container/foreach.h>
#include <cx/container/sarray.h>
#include <cx/debug/crash.h>
#include <cx/thread/mutex.h>
#include <cx/utils/lazyinit.h>

static Mutex _xaOOMMutex;

static LazyInitState _xaOOMInitState;
static bool useoom;

saDeclare(xaOOMCallback);
static sa_xaOOMCallback callbacks;

static void _xaOOMInit(void *data)
{
    mutexInit(&_xaOOMMutex);
    saInit(&callbacks, ptr, 8);
    useoom = true;
}

void xaAddOOMCallback(xaOOMCallback cb)
{
    lazyInit(&_xaOOMInitState, _xaOOMInit, 0);
    withMutex(&_xaOOMMutex)
    {
        saPush(&callbacks, ptr, cb, SA_Unique);
    }
}

void xaRemoveOOMCallback(xaOOMCallback cb)
{
    lazyInit(&_xaOOMInitState, _xaOOMInit, 0);
    withMutex(&_xaOOMMutex)
    {
        saFindRemove(&callbacks, ptr, cb);
    }
}

void _xaFreeUpMemory(int phase, size_t allocsz)
{
    // Don't try to init when we're already in a bad state, just exit.
    // This means there aren't any callbacks anyway.
    if (!useoom)
        return;

    withMutex(&_xaOOMMutex)
    {
        foreach(sarray, i, xaOOMCallback, callback, callbacks)
        {
            callback(phase, allocsz);
        }
    }

    // in high effort and urgent phases, compact memory and return as much to the OS as possible
    // in the hopes it will become available for the allocator (i.e. if it was in some other
    // thread's sharded freelist)
    if (phase >= XA_HighEffort)
        xaFlush();
}

_When_(!(flags & XA_Optional_Mask), _Analysis_noreturn_)
void _xaAllocFailure(size_t allocsz, unsigned int flags)
{
    if (flags & XA_Optional_Mask)
        return;

   _xaFreeUpMemory(XA_Fatal, allocsz);      // notify OOM handlers we're about to crash
   dbgCrashNow(0);
}
