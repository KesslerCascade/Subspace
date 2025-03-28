#pragma once

#include <cx/cx.h>
#include <cx/platform/win.h>

_meta_inline int64 timeFromFileTime(FILETIME *ft)
{
    int64 ret = ((int64)ft->dwHighDateTime << 32) | ft->dwLowDateTime;
    ret /= 10;          // convert from 100-ns intervals to microseconds

    // FILETIME epoch is midnight on Jan 1, 1601
    // Which is a julian date of 2305813.50000
    // That's 199222286400 in seconds, or in microseconds...

    ret += 199222286400000000LL;        // adjust epoch
    // This still leaves over 280,000 years before overflow

    return ret;
}

_meta_inline bool timeToFileTime(int64 time, FILETIME *ft)
{
    time -= 199222286400000000LL;       // adjust epoch
    if (time < 0)
        return false;

    time *= 10;         // convert to 100-ns intervals
    ft->dwHighDateTime = (DWORD)(time >> 32);
    ft->dwLowDateTime = (DWORD)(time & 0xffffffff);
    return true;
}
