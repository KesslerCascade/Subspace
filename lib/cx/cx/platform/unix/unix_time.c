#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include "cx/time/clock.h"
#include "cx/time/time.h"

int64 timeLocal(int64 time, int64 *offset)
{
    // get the correct offset for the given date
    time_t ttime = timeToTimeT(time);
    struct tm dummytm;
    localtime_r(&ttime, &dummytm);

    int64 off = timeFromSeconds(dummytm.tm_gmtoff);
    if (offset)
        *offset = off;

    return time + off;
}
