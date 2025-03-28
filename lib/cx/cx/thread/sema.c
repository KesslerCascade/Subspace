#include "sema.h"
#include <cx/time/clock.h>
#include <cx/platform/cpu.h>
#include <cx/platform/os.h>
#include <cx/utils/compare.h>

_Use_decl_annotations_
void _semaInit(Semaphore *sema, int32 count, uint32 flags)
{
    devAssert(count >= 0);
    memset(sema, 0, sizeof(Semaphore));
    futexInit(&sema->ftx, count);
    aspinInit(&sema->aspin, flags & SEMA_NoSpin);
}

_Use_decl_annotations_
bool semaDestroy(Semaphore *sema)
{
    devAssert(atomicLoad(int32, &sema->ftx.val, Acquire) >= 0);
    memset(sema, 0, sizeof(Semaphore));

    return true;
}

_Use_decl_annotations_
bool semaTryDecTimeout(Semaphore *sema, int64 timeout)
{
    int32 val = atomicLoad(int32, &sema->ftx.val, Relaxed);
    if (val > 0 && atomicCompareExchange(int32, strong, &sema->ftx.val, &val,
                                         val - 1, Acquire, Relaxed)) {
        aspinRecordUncontended(&sema->aspin);
        return true;
    }

    AdaptiveSpinState astate;
    aspinBegin(&sema->aspin, &astate, timeout);
    while (val == 0 || !atomicCompareExchange(int32, weak, &sema->ftx.val, &val, val - 1, Acquire, Relaxed)) {
        if (aspinTimeout(&sema->aspin, &astate))
            return false;

        if (val == 0) {
            if (!aspinSpin(&sema->aspin, &astate))
                futexWait(&sema->ftx, val, aspinTimeoutRemaining(&astate));
            val = atomicLoad(int32, &sema->ftx.val, Relaxed);
        } else {
            aspinHandleContention(&sema->aspin, &astate);
        }
    }

    aspinAdapt(&sema->aspin, &astate);

    return true;
}
