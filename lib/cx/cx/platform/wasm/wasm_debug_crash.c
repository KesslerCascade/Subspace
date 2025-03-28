#include "cx/debug/crash_private.h"

bool _dbgCrashPlatformInit()
{
    return true;
}

_no_inline _no_return void dbgCrashNow(int skip)
{
    lazyInit(&_dbgCrashInitState, _dbgCrashInit, 0);

//    stframes = dbgStackTrace(skip + 1, ST_MAX_FRAMES, stacktrace);
    __builtin_trap();
}

