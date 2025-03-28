#pragma once

#include "cx/cx.h"
#include <time.h>

#define timeForever 9223372036854775807LL
#define timeS(s) timeFromSeconds(s)
#define timeMS(s) timeFromMsec(s)

enum WEEKDAYS {
    Sunday = 0,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday
};
extern strref timeDayName[];
extern strref timeDayAbbrev[];
extern strref timeMonthName[];
extern strref timeMonthAbbrev[];

typedef struct TimeParts {
    uint32 usec;    // first for structure alignment reasons
    int32 year;     // Negative numbers are used for BC (there is no year 0)
    uint8 month;
    uint8 day;
    uint8 hour;
    uint8 minute;
    uint8 second;

    // these parts are for information only and not used for composition
    uint8 wday;     // day of week
    uint16 yday;    // day of year
} TimeParts;

bool timeDecompose(_Out_ TimeParts *out, _In_range_(0, timeForever) int64 time);

_Ret_range_(0, timeForever)
int64 timeCompose(_In_ TimeParts *parts);

// convert a time to local time, optionally returning the offset
_Success_(return > 0)
int64 timeLocal(int64 time, _Out_opt_ int64 *offset);

// time interval conversion
_meta_inline int64 timeToSeconds(int64 time)
{
    return time / 1000000;
}

_meta_inline int64 timeFromSeconds(int64 seconds)
{
    return seconds * 1000000;
}

_meta_inline int64 timeToMsec(int64 time)
{
    return time / 1000;
}

_meta_inline int64 timeFromMsec(int64 msec)
{
    return msec * 1000;
}

// relative timespec

_Ret_range_(0, timeForever)
_meta_inline int64 timeFromRelTimespec(_In_ struct timespec *ts)
{
    int64 ret = (int64)ts->tv_sec * 1000000;
    ret += ts->tv_nsec / 1000;
    return ret;
}

_meta_inline void timeToRelTimespec(_Out_ struct timespec *ts, int64 time)
{
    ts->tv_sec = time / 1000000;
    ts->tv_nsec = (time % 1000000) * 1000;
}

// unix absolute timespec

_Ret_range_(0, timeForever)
_meta_inline int64 timeFromAbsTimespec(_In_ struct timespec *ts)
{
    int64 ret = (int64)ts->tv_sec * 1000000;
    ret += ts->tv_nsec / 1000;

    // Unix epoch is midnight on Jan 1, 1970
    // Which is a julian date of 2440587.50000
    // That's 210866760000 in seconds, or in microseconds...

    ret += 210866760000000000LL;       // adjust epoch

    // This still leaves us over 280,000 years before overflow

    return ret;
}

_meta_inline void timeToAbsTimespec(_Out_ struct timespec *ts, int64 time)
{
    time -= 210866760000000000LL;       // adjust epoch

    ts->tv_sec = time / 1000000;
    ts->tv_nsec = (time % 1000000) * 1000;
}

// unix time_t

_Ret_range_(0, timeForever)
_meta_inline int64 timeFromTimeT(time_t tt)
{
    int64 ret = (int64)tt * 1000000;
    ret += 210866760000000000LL;       // adjust epoch
    return ret;
}

_meta_inline time_t timeToTimeT(int64 time)
{
    time -= 210866760000000000LL;       // adjust epoch
    return (time_t)(time / 1000000);
}
