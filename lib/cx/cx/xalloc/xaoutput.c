#include <cx/cx.h>
#include "xalloc_private.h"

#include <cx/log/log_private.h>
#include <cx/string/strmanip.h>

#ifndef XALLOC_USE_SYSTEM_MALLOC

static void xaMimallocOutput(const char *msg, void *arg)
{
    static _Thread_local int lastLevel = LOG_Warn;

    // This can get called during mi_register_output. Be careful not to actually output anything in that case
    // because _logStr allocates and recursing into xaInit causes a deadlock with ourselves.
    if (atomicLoad(bool, (atomic(bool) *) & _xaInitState.initProgress, Acquire) &&
        !atomicLoad(bool, (atomic(bool) *) & _xaInitState.init, Acquire))
        return;

    if (msg[0] == 0 || msg[0] == '\n')
        return;

    // mimalloc output function is kind of annoying, it sends the prefix and message in two separate calls.
    // it may also append a thread ID to the prefix
    if (!strncmp(msg, "mimalloc: error: ", 17)) {
        lastLevel = LOG_Error;
        return;
    } else if (!strncmp(msg, "mimalloc: warning: ", 19)) {
        lastLevel = LOG_Warn;
        return;
    } else if (!strncmp(msg, "mimalloc: ", 10)) {
        lastLevel = LOG_Verbose;
        return;
    }

    string tmp = 0;
    strTemp(&tmp, 256);
    strDup(&tmp, (string)msg);
    uint8 ch = strGetChar(tmp, -1);

    if (ch != '\n')
        goto out;

    strSetLen(&tmp, strLen(tmp) - 1);           // chop off \n
    _logStr(lastLevel, LogDefault, tmp);

out:
    strDestroy(&tmp);
}

void _xaInitOutput(void)
{
    mi_register_output(xaMimallocOutput, NULL);

    // Make sure we weren't called from logInit to begin with, if so just exit
    if (atomicLoad(bool, (atomic(bool) *) & _logInitState.initProgress, Acquire) &&
        !atomicLoad(bool, (atomic(bool) *) & _logInitState.init, Acquire))
        return;

    // Init logging now
    // This short-circuits xaInit first since we know logCheckInit will call
    // xaAlloc, and we need to make sure it doesn't recurse. Otherwise a deadlock
    // would be possible if the very first xalloc API usage was inside of an init
    // function and also caused a warning message to be printed.

    atomicStore(bool, (atomic(bool) *) & _xaInitState.init, true, Relaxed);
    logCheckInit();
}

#endif
