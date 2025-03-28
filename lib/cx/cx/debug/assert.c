#include "cx/debug/assert.h"
#include "cx/debug/crash.h"
#include <cx/container/foreach.h>
#include <cx/container/sarray.h>
#include <cx/thread/mutex.h>
#include <cx/utils/lazyinit.h>
#include <stdlib.h>

static Mutex _dbgAssertMutex;

static LazyInitState _dbgAssertInitState;

saDeclare(dbgAssertCallback);
static sa_dbgAssertCallback callbacks;

static void _dbgAssertInit(void *data)
{
    mutexInit(&_dbgAssertMutex);
    saInit(&callbacks, ptr, 0);
}

void dbgAssertAddCallback(dbgAssertCallback cb)
{
    lazyInit(&_dbgAssertInitState, _dbgAssertInit, 0);
    withMutex(&_dbgAssertMutex) {
        saPush(&callbacks, ptr, cb, SA_Unique);
    }
}

void dbgAssertRemoveCallback(dbgAssertCallback cb)
{
    lazyInit(&_dbgAssertInitState, _dbgAssertInit, 0);
    withMutex(&_dbgAssertMutex) {
        saFindRemove(&callbacks, ptr, cb);
    }
}

static int dbgAssertTriggerCallbacks(_In_opt_z_ const char *expr, _In_opt_z_ const char *msg, _In_opt_z_ const char *file, int ln)
{
    int ret = ASSERT_Crash;
    // caller should be holding mutex
    foreach(sarray, i, dbgAssertCallback, callback, callbacks)
    {
        int cret = callback(expr, msg, file, ln);
        // if callback wants to crash or terminate, do it now
        if (cret == ASSERT_Crash || cret == ASSERT_Exit)
            return cret;

        // else if it wants to ignore, do it only if all other callbacks agree
        ret = cret;
    }

    return ret;
}

_Use_decl_annotations_
#if DEBUG_LEVEL >= 1
_no_inline bool _cxAssertFail(const char *expr, const char *msg, const char *file, int ln)
#else
_no_inline bool _cxAssertFail(const char *expr, const char *msg)
#endif
{
    lazyInit(&_dbgAssertInitState, _dbgAssertInit, 0);

#if DEBUG_LEVEL >= 1
    int action = dbgAssertTriggerCallbacks(expr, msg, file, ln);
#else
    int action = dbgAssertTriggerCallbacks(expr, msg, NULL, 0);
#endif

    if (action == ASSERT_Ignore)
        return false;               // the expression evaluated to false, be sure to return the same
    if (action == ASSERT_Exit)
        exit(1);

    // ASSERT_Crash fallthrough

    if (expr)
        dbgCrashAddMetaStr("assertexpr", expr);
    if (msg)
        dbgCrashAddMetaStr("assertmsg", msg);
#if DEBUG_LEVEL >= 1
    dbgCrashAddMetaStr("assertfile", file);
    dbgCrashAddMetaInt("assertline", ln);
#endif
    dbgCrashNow(1);
    return false;
}
