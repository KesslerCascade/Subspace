#include "rwlock.h"

// Reader/writer lock implementation

// Inspired by Jeff Preshing's semaphore-based RW locks
// https://github.com/preshing/cpp11-on-multicore/blob/master/common/rwlock.h

// Reworked to support timeouts for both read and write lock acquisition. Also
// does not assert if the maximum number of readers, waiters, or writers is
// reached but rather fails to acquire the lock.

// Further reworked to use futexes instead of semaphores.

_Use_decl_annotations_
void _rwlockInit(RWLock *l, uint32 flags)
{
    atomicStore(uint32, &l->state, 0, Relaxed);
    futexInit(&l->rftx, 0);
    futexInit(&l->wftx, 0);
    aspinInit(&l->aspin, flags & RWLOCK_NoSpin);
}

_Use_decl_annotations_
bool rwlockTryAcquireReadTimeout(RWLock *l, int64 timeout)
{
    uint32 state = atomicLoad(uint32, &l->state, Relaxed);
    uint32 nstate;
    // first register as either a reader or a waiting reader
    for (;;) {
        if (RWLOCK_WRITERS(state) > 0) {
            // if there are any writers we must go into readwait state
            if (RWLOCK_READWAIT(state) == RWLOCK_READWAIT_MAX)
                return false;           // too many already
            nstate = state + RWLOCK_READWAIT_ADD;

            // try to update the new state
            if (atomicCompareExchange(uint32, weak, &l->state, &state, nstate, Relaxed, Relaxed))
                break;
            else
                osYield();      // we're likely to have to wait anyway
        } else {
            // otherwise we're just a normal read lock
            if (RWLOCK_READERS(state) == RWLOCK_READER_MAX)
                return false;           // too many already
            nstate = state + RWLOCK_READ_ADD;

            // try to update the new state, there should be no conflicting writers on this path
            if (atomicCompareExchange(uint32, weak, &l->state, &state, nstate, Acquire, Relaxed))
                break;
        }
    }

    // if there weren't any writers on the state we successfully swapped, then we're done
    if (RWLOCK_WRITERS(state) == 0) {
        aspinRecordUncontended(&l->aspin);
        return true;
    }

    // otherwise have to wait in the read queue until released
    AdaptiveSpinState astate;
    aspinBegin(&l->aspin, &astate, timeout);
    int rval = atomicLoad(int32, &l->rftx.val, Relaxed);
    while (rval == 0 || !atomicCompareExchange(int32, weak, &l->rftx.val, &rval, rval - 1, Relaxed, Relaxed))
    {
        if (aspinTimeout(&l->aspin, &astate)) {
            // have to undo the add of RWLOCK_READWAIT_ADD from above,
            // but be careful to avoid a race with rwlockReleaseWrite
            state = atomicLoad(uint32, &l->state, Relaxed);
            while (RWLOCK_READWAIT(state) > 0 &&
                   !atomicCompareExchange(uint32, weak, &l->state, &state, state - RWLOCK_READWAIT_ADD, Relaxed, Relaxed)) {
                aspinHandleContention(&l->aspin, &astate);
            }
            return false;
        }

        if (rval == 0) {
            if (!aspinSpin(&l->aspin, &astate))
                futexWait(&l->rftx, rval, aspinTimeoutRemaining(&astate));
            rval = atomicLoad(int32, &l->rftx.val, Relaxed);
        }
    }

    // got a read lock finally
    aspinAdapt(&l->aspin, &astate);
    return true;
}

_Use_decl_annotations_
bool rwlockTryAcquireWriteTimeout(RWLock *l, int64 timeout)
{
    uint32 state = atomicLoad(uint32, &l->state, Relaxed);
    // Try lightweight path first if there's no contention and the lock is wide open
    if (RWLOCK_WRITERS(state) == 0 && RWLOCK_READERS(state) == 0) {
        if (atomicCompareExchange(uint32, strong, &l->state, &state, state + RWLOCK_WRITE_ADD, Acquire, Relaxed)) {
            aspinRecordUncontended(&l->aspin);
            return true;        // got the lock
        }
    }

    AdaptiveSpinState astate;
    aspinBegin(&l->aspin, &astate, timeout);
    for (;;) {
        // make sure we didn't hit the limit
        if (RWLOCK_WRITERS(state) == RWLOCK_WRITER_MAX)
            return false;

        if (atomicCompareExchange(uint32, weak, &l->state, &state, state + RWLOCK_WRITE_ADD, Relaxed, Relaxed))
            break;
    }

    // if there were any other readers or writers we must wait on them
    if (RWLOCK_READERS(state) > 0 || RWLOCK_WRITERS(state) > 0) {
        // wait in the write queue
        int32 wval = atomicLoad(int32, &l->wftx.val, Relaxed);
        while (wval == 0 || !atomicCompareExchange(int32, weak, &l->wftx.val, &wval, wval - 1, Relaxed, Relaxed)) {
            if (aspinTimeout(&l->aspin, &astate)) {
                // nothing else can remove a writer except the writer itself, so we can just subtract in this case
                // (see comments in rwlockTryAcquireReadTimeout)
                atomicFetchSub(uint32, &l->state, RWLOCK_WRITE_ADD, Relaxed);
                return false;
            }

            if (wval == 0) {
                if (!aspinSpin(&l->aspin, &astate))
                    futexWait(&l->wftx, wval, aspinTimeoutRemaining(&astate));
                wval = atomicLoad(int32, &l->wftx.val, Relaxed);
            } else {
                aspinHandleContention(&l->aspin, &astate);
            }
        }

        aspinAdapt(&l->aspin, &astate);
    }

    return true;
}

_meta_inline bool _rwlockReleaseWriteInternal(_Inout_ RWLock *l, bool downgrade)
{
    uint32 state = atomicLoad(uint32, &l->state, Relaxed);
    uint32 nstate, rwait;

    do {
        devAssert(RWLOCK_WRITERS(state) > 0 && RWLOCK_READERS(state) == 0);
        if (RWLOCK_WRITERS(state) == 0)
            return false;

        // normally we just subtract a writer
        nstate = state - RWLOCK_WRITE_ADD;

        // but if any threads are waiting to read...
        rwait = RWLOCK_READWAIT(state);
        if (rwait > 0 || downgrade) {
            // build a new state:
            //   writers  is carried over from the old state (minus 1)
            //   readwait is set to 0
            //   readers  is set to what was in readwait
            //     (readwait does not need to be masked because it
            //      is less bits wide than readers)
            //     ...plus one if this is a downgrade unlock
            nstate = (nstate & RWLOCK_WRITER_MASK) | (rwait + (downgrade ? 1 : 0));
        }

        // don't use aspinHandleContention; releasing write lock should be top priority
    } while (!atomicCompareExchange(uint32, weak, &l->state, &state, nstate, Release, Relaxed));

    if (rwait > 0 || downgrade) {
        // if any threads are waiting to read, release them
        // waiting readers get priority over writers so that they take turns
        atomicFetchAdd(int32, &l->rftx.val, rwait, Relaxed);
        futexWakeAll(&l->rftx);
    } else if (RWLOCK_WRITERS(state) > 1) {
        // otherwise if anyone else is waiting, release one thread
        atomicFetchAdd(int32, &l->wftx.val, 1, Relaxed);
        futexWake(&l->wftx);
    }

    return true;
}

_Use_decl_annotations_
bool rwlockReleaseWrite(RWLock *l)
{
    return _rwlockReleaseWriteInternal(l, false);
}

_Use_decl_annotations_
bool rwlockDowngradeWrite(RWLock *l)
{
    return _rwlockReleaseWriteInternal(l, true);
}

_Use_decl_annotations_
void rwlockDestroy(RWLock *l)
{
    memset(l, 0, sizeof(RWLock));
}
