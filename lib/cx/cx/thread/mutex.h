#pragma once

#include <cx/cx.h>
#include <cx/time/time.h>
#include <cx/meta/block.h>
#include <cx/utils/macros.h>
#include "futex.h"
#include "aspin.h"

#ifdef CX_LOCK_DEBUG
#include <cx/log/log.h>
#endif

CX_C_BEGIN

enum MUTEX_Flags {
    MUTEX_NoSpin = 1,           // do not use adaptive spin, use kernel futex only
};

typedef struct Mutex {
    // Futex values:
    // 0 - Lock is not held
    // 1 - Lock is held
    // 2 - Lock is held and there is contention
    Futex ftx;
    AdaptiveSpin aspin;
} Mutex;

void _mutexInit(_Out_ Mutex *m, uint32 flags);
#define mutexInit(m, ...) _mutexInit(m, opt_flags(__VA_ARGS__))

_When_(return == true, _Acquires_nonreentrant_lock_(*m))
_When_(timeout == timeForever, _Acquires_nonreentrant_lock_(*m))
_When_(timeout != timeForever, _Must_inspect_result_)
bool mutexTryAcquireTimeout(_Inout_ Mutex *m, int64 timeout);

_Releases_nonreentrant_lock_(*m)
bool mutexRelease(_Inout_ Mutex *m);

_When_(return == true, _Acquires_nonreentrant_lock_(*m))
_Must_inspect_result_
_meta_inline bool mutexTryAcquire(_Inout_ Mutex *m)
{
    int32 curstate = atomicLoad(int32, &m->ftx.val, Relaxed);
    if (curstate == 0 && atomicCompareExchange(int32, strong, &m->ftx.val, &curstate, 1, Acquire, Relaxed)) {
        aspinRecordUncontended(&m->aspin);
        return true;
    }
    return false;
}

_Acquires_nonreentrant_lock_(*m)
_meta_inline void mutexAcquire(_Inout_ Mutex *m)
{
    mutexTryAcquireTimeout(m, timeForever);
}

#define withMutex(m) blkWrap(mutexAcquire(m), mutexRelease(m))

#ifdef CX_LOCK_DEBUG
#define _logFmtMutexArgComp2(level, fmt, nargs, args) _logFmt_##level(LOG_##level, LogDefault, fmt, nargs, args)
#define _logFmtMutexArgComp(level, fmt, ...)          _logFmtMutexArgComp2(level, fmt, count_macro_args(__VA_ARGS__), (stvar[]){ __VA_ARGS__ })
_Acquires_nonreentrant_lock_(*m)
_meta_inline bool mutexLogAndAcquire(_Inout_ Mutex *m, const char *name, const char *filename, int line)
{
    _logFmtMutexArgComp(CX_LOCK_DEBUG, _S"Locking mutex ${string} at ${string}:${int}",
                        stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return mutexAcquire(m);
}

#define mutexAcquire(m) mutexLogAndAcquire(m, #m, __FILE__, __LINE__)
#endif

#ifdef CX_LOCK_DEBUG
_Releases_nonreentrant_lock_(*m)
_meta_inline bool mutexLogAndRelease(_Inout_ Mutex *m, const char *name, const char *filename, int line)
{
    _logFmtMutexArgComp(CX_LOCK_DEBUG, _S"Releasing mutex ${string} at ${string}:${int}",
                        stvar(string, (string)name), stvar(string, (string)filename), stvar(int32, line));
    return mutexRelease(m);
}

#define mutexRelease(m) mutexLogAndRelease(m, #m, __FILE__, __LINE__)
#endif

void mutexDestroy(_Pre_valid_ _Post_invalid_ Mutex *m);

CX_C_END
