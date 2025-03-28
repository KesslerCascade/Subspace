#include "condvar.h"

_Use_decl_annotations_
void _cvarInit(CondVar *cv, uint32 flags)
{
    futexInit(&cv->seq, 0);
    atomicStore(uint32, &cv->lastseq, 0, Relaxed);
    aspinInit(&cv->aspin, flags & CONDVAR_NoSpin);
}

_Use_decl_annotations_
void cvarDestroy(CondVar *cv)
{
    memset(cv, 0, sizeof(CondVar));
}

_Use_decl_annotations_
bool cvarWaitTimeout(CondVar *cv, Mutex *m, int64 timeout)
{
    int32 seq = atomicLoad(int32, &cv->seq.val, Relaxed);
    int32 lastseq = seq;
    atomicStore(uint32, &cv->lastseq, (uint32)lastseq, Relaxed);

    mutexRelease(m);
    AdaptiveSpinState astate;
    aspinBegin(&cv->aspin, &astate, timeout);
    // just wait for seq val to change
    while (seq == lastseq) {
        if (aspinTimeout(&cv->aspin, &astate))
            return false;

        if (!aspinSpin(&cv->aspin, &astate))
            futexWait(&cv->seq, lastseq, aspinTimeoutRemaining(&astate));

        seq = atomicLoad(int32, &cv->seq.val, Relaxed);
    }
    aspinAdapt(&cv->aspin, &astate);

    // Re-acquire the mutex
    // NOTE: This can lead to a lot of contention when cvarBroadcast is used.
    // On Linux there is the futex re-queue operation which is designed
    // specifically to handle this better. Other platforms don't have that
    // however. TODO: Evaluate if it's worth the necessary changes to the
    // condvar algorithm to take advantage of that on Linux only.

    if (timeout == timeForever) {
        mutexAcquire(m);
        return true;
    } else {
        if (aspinTimeout(&cv->aspin, &astate))
            return false;
        return mutexTryAcquireTimeout(m, aspinTimeoutRemaining(&astate));
    }
}

_Use_decl_annotations_
bool cvarSignal(CondVar *cv)
{
    uint32 seq = atomicLoad(uint32, &cv->lastseq, Relaxed) + 1;
    atomicStore(int32, &cv->seq.val, (int32)seq, Relaxed);

    futexWake(&cv->seq);
    return true;
}

_Use_decl_annotations_
bool cvarBroadcast(CondVar *cv)
{
    uint32 seq = atomicLoad(uint32, &cv->lastseq, Relaxed) + 1;
    atomicStore(int32, &cv->seq.val, (int32)seq, Relaxed);

    futexWakeAll(&cv->seq);
    return true;
}
