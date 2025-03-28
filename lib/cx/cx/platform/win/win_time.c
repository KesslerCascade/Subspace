#include "win_time.h"
#include "cx/platform/win.h"
#include "cx/time/time.h"

_Use_decl_annotations_
int64 timeLocal(int64 time, int64 *offset)
{
    FILETIME ft, lt;
    if (!timeToFileTime(time, &ft) || !FileTimeToLocalFileTime(&ft, &lt))
        return 0;

    int64 ret = timeFromFileTime(&lt);
    if (offset)
        *offset = ret - time;

    return ret;
}
