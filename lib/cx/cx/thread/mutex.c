#include "mutex.h"
#include <cx/time/clock.h>

_Use_decl_annotations_
void _mutexInit(Mutex *m, uint32 flags)
{
    futexInit(&m->ftx, 0);
    aspinInit(&m->aspin, flags & MUTEX_NoSpin);
}

_Use_decl_annotations_
bool mutexTryAcquireTimeout(Mutex *m, int64 timeout)
{
    // try simple lock first
    int32 curstate = atomicLoad(int32, &m->ftx.val, Relaxed);
    if (curstate == 0 && atomicCompareExchange(int32, strong, &m->ftx.val, &curstate, 1, Acquire, Relaxed)) {
        aspinRecordUncontended(&m->aspin);
        return true;
    }

    // nope, have to wait
    AdaptiveSpinState astate;
    aspinBegin(&m->aspin, &astate, timeout);
    do {
        do {
            if (curstate == 2 || (curstate == 1 && atomicCompareExchange(int32, weak, &m->ftx.val, &curstate, 2, Relaxed, Relaxed))) {
                if (!aspinSpin(&m->aspin, &astate))
                    futexWait(&m->ftx, 2, aspinTimeoutRemaining(&astate));
            }

            curstate = atomicLoad(int32, &m->ftx.val, Relaxed);
            if (curstate != 2)          // the CAS above failed
                aspinHandleContention(&m->aspin, &astate);

            if (aspinTimeout(&m->aspin, &astate))
                return false;
        } while (curstate != 0);

        // try to lock it again
    } while (!atomicCompareExchange(int32, strong, &m->ftx.val, &curstate, 2, Acquire, Relaxed));

    aspinAdapt(&m->aspin, &astate);
    return true;
}

_Use_decl_annotations_
bool mutexRelease(Mutex *m)
{
    int prevstate = atomicFetchSub(int32, &m->ftx.val, 1, Release);
    devAssert(prevstate > 0);
    if (prevstate > 1) {
        futexSet(&m->ftx, 0);
        futexWake(&m->ftx);
        return true;
    }
    return (prevstate > 0);
}

_Use_decl_annotations_
void mutexDestroy(Mutex *m)
{
    memset(m, 0, sizeof(Mutex));
}
