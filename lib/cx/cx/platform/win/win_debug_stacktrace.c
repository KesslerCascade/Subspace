#include "win_stacktrace.h"
#include "cx/platform/win.h"

_no_inline int dbgStackTrace(int nskip, int nframes, uintptr_t *addrs)
{
    return RtlCaptureStackBackTrace(nskip + 1, nframes, (void**)addrs, NULL);
}
