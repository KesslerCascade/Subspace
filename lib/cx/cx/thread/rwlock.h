#pragma once

#include <cx/cx.h>
#include <cx/platform/base.h>
#include <cx/meta/block.h>
#include "futex.h"
#include "aspin.h"

#ifdef CX_LOCK_DEBUG
#include <cx/log/log.h>
#endif

CX_C_BEGIN

enum RWLOCK_Flags {
    RWLOCK_NoSpin = 1,          // do not use adaptive spin, use kernel futex only
};

#define RWLOCK_READER_MAX 4095
#define RWLOCK_READWAIT_MAX 2047
#define RWLOCK_WRITER_MAX 511

#define RWLOCK_READER_MASK              (0x00000fff)
#define RWLOCK_READWAIT_MASK            (0x007ff000)
#define RWLOCK_WRITER_MASK              (0xff800000)
#define RWLOCK_READERS(state)   ((state) & RWLOCK_READER_MASK)
#define RWLOCK_READWAIT(state) (((state) & RWLOCK_READWAIT_MASK) >> 12)
#define RWLOCK_WRITERS(state)  (((state) & RWLOCK_WRITER_MASK)   >> 23)
#define RWLOCK_READ_ADD                  0x00000001
#define RWLOCK_READWAIT_ADD              0x00001000
#define RWLOCK_WRITE_ADD                 0x00800000

typedef struct RWLock {
    atomic(uint32) state;   // main lock state
    Futex rftx;             // read queue
    Futex wftx;             // write queue
    AdaptiveSpin aspin;
} RWLock;

void _rwlockInit(_Out_ RWLock *l, uint32 flags);
#define rwlockInit(l, ...) _rwlockInit(l, opt_flags(__VA_ARGS__))

_When_(return == true, _Acquires_shared_lock_(*l))
_When_(timeout == timeForever, _Acquires_shared_lock_(*l))
_When_(timeout != timeForever, _Must_inspect_result_)
bool rwlockTryAcquireReadTimeout(_Inout_ RWLock *l, int64 timeout);

_When_(return == true, _Acquires_exclusive_lock_(*l))
_When_(timeout == timeForever, _Acquires_exclusive_lock_(*l))
_When_(timeout != timeForever, _Must_inspect_result_)
bool rwlockTryAcquireWriteTimeout(_Inout_ RWLock *l, int64 timeout);

_When_(return == true, _Acquires_shared_lock_(*l))
_Must_inspect_result_
_meta_inline bool rwlockTryAcquireRead(_Inout_ RWLock *l)
{
    uint32 state = atomicLoad(uint32, &l->state, Relaxed);
    // only valid when no writer locks are held or pending
    if (RWLOCK_WRITERS(state) == 0) {
        // cannot acquire if we are at the max
        if (RWLOCK_READERS(state) == RWLOCK_READER_MAX)
            return false;
        if (atomicCompareExchange(uint32, strong, &l->state, &state, state + RWLOCK_READ_ADD, Acquire, Relaxed)) {
            aspinRecordUncontended(&l->aspin);
            return true;        // got the lock
        }
    }

    // no longer have valid conditions in which this lock can be acquired
    return false;
}

_When_(return == true, _Acquires_exclusive_lock_(*l))
_Must_inspect_result_
_meta_inline bool rwlockTryAcquireWrite(_Inout_ RWLock *l)
{
    uint32 state = atomicLoad(uint32, &l->state, Relaxed);
    // only valid when no other writer locks are held, and there are no (active) readers
    if (RWLOCK_WRITERS(state) == 0 && RWLOCK_READERS(state) == 0) {
        // make sure we didn't hit the limit
        if (RWLOCK_WRITERS(state) == RWLOCK_WRITER_MAX)
            return false;
        if (atomicCompareExchange(uint32, strong, &l->state, &state, state + RWLOCK_WRITE_ADD, Acquire, Relaxed)) {
            aspinRecordUncontended(&l->aspin);
            return true;        // got the lock
        }
    }

    // no longer have valid conditions in which this lock can be acquired
    return false;
}

_Acquires_shared_lock_(*l)
_meta_inline void rwlockAcquireRead(_Inout_ RWLock *l)
{
    if (!rwlockTryAcquireReadTimeout(l, timeForever))
        relFatalError("Failed to acquire read lock (too many waiting readers?)");
}

#define withReadLock(l) blkWrap(rwlockAcquireRead(l), rwlockReleaseRead(l))
#define withWriteLock(l) blkWrap(rwlockAcquireWrite(l), rwlockReleaseWrite(l))

#ifdef CX_LOCK_DEBUG
#define _logFmtRwlockArgComp2(level, fmt, nargs, args) _logFmt_##level(LOG_##level, LogDefault, fmt, nargs, args)
#define _logFmtRwlockArgComp(level, fmt, ...)          _logFmtRwlockArgComp2(level, fmt, count_macro_args(__VA_ARGS__), (stvar[]){ __VA_ARGS__ })
_Acquires_shared_lock_(*l)
_meta_inline bool rwlockLogAndAcquireRead(_Inout_ RWLock *l, const char *name, const char *filename, int line)
{
    _logFmtRwlockArgComp(CX_LOCK_DEBUG, _S"Locking rwlock ${string} for READ at ${string}:${int}",
                         stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return rwlockAcquireRead(l);
}

#define rwlockAcquireRead(l) rwlockLogAndAcquireRead(l, #l, __FILE__, __LINE__)
#endif

_Acquires_exclusive_lock_(*l)
_meta_inline void rwlockAcquireWrite(_Inout_ RWLock *l)
{
    if (!rwlockTryAcquireWriteTimeout(l, timeForever))
        relFatalError("Failed to acquire write lock (too many waiting writers?)");
}

#ifdef CX_LOCK_DEBUG
_Acquires_exclusive_lock_(*l)
_meta_inline bool rwlockLogAndAcquireWrite(_Inout_ RWLock *l, const char *name, const char *filename, int line)
{
    _logFmtRwlockArgComp(CX_LOCK_DEBUG, _S"Locking rwlock ${string} for WRITE at ${string}:${int}",
                         stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return rwlockAcquireWrite(l);
}

#define rwlockAcquireWrite(l) rwlockLogAndAcquireWrite(l, #l, __FILE__, __LINE__)
#endif

_Releases_shared_lock_(*l)
_meta_inline bool rwlockReleaseRead(_Inout_ RWLock *l)
{
    devAssert(RWLOCK_READERS(atomicLoad(uint32, &l->state, Relaxed)) > 0);
    uint32 oldstate = atomicFetchSub(uint32, &l->state, RWLOCK_READ_ADD, Release);

    // If we were the last reader and any writers are waiting, unblock one
    if (RWLOCK_READERS(oldstate) == 1 && RWLOCK_WRITERS(oldstate) > 0) {
        devVerify(atomicFetchAdd(int32, &l->wftx.val, 1, Relaxed) == 0);
        futexWake(&l->wftx);
    }

    return true;
}

#ifdef CX_LOCK_DEBUG
_Releases_shared_lock_(*l)
_meta_inline bool rwlockLogAndReleaseRead(_Inout_ RWLock *l, const char *name, const char *filename, int line)
{
    _logFmtRwlockArgComp(CX_LOCK_DEBUG, _S"Releasing rwlock ${string} for READ at ${string}:${int}",
                         stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return rwlockReleaseRead(l);
}

#define rwlockReleaseRead(l) rwlockLogAndReleaseRead(l, #l, __FILE__, __LINE__)
#endif

_Releases_exclusive_lock_(*l)
bool rwlockReleaseWrite(_Inout_ RWLock *l);

#ifdef CX_LOCK_DEBUG
_Releases_exclusive_lock_(*l)
_meta_inline bool rwlockLogAndReleaseWrite(_Inout_ RWLock *l, const char *name, const char *filename, int line)
{
    _logFmtRwlockArgComp(CX_LOCK_DEBUG, _S"Releasing rwlock ${string} for WRITE at ${string}:${int}",
                         stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return rwlockReleaseWrite(l);
}

#define rwlockReleaseWrite(l) rwlockLogAndReleaseWrite(l, #l, __FILE__, __LINE__)
#endif

// Atomically release write lock and acquire read lock, guaranteed not to wait
_Releases_exclusive_lock_(*l)
_Acquires_shared_lock_(*l)
bool rwlockDowngradeWrite(_Inout_ RWLock *l);

void rwlockDestroy(_Pre_valid_ _Post_invalid_ RWLock *l);

CX_C_END
