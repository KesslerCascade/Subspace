#ifdef __STRICT_ANSI__
#undef __STRICT_ANSI__
#endif

#include "cx/time/clock.h"
#include "cx/time/time.h"

int64 clockWall()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return timeFromAbsTimespec(&ts);
}

int64 clockWallLocal()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    struct tm dummytm;
    time_t curtime = time(NULL);
    localtime_r(&curtime, &dummytm);
    ts.tv_sec += dummytm.tm_gmtoff;

    return timeFromAbsTimespec(&ts);
}

int64 clockTimer()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    int64 ret = (int64)ts.tv_sec * 1000000;
    ret += ts.tv_nsec / 1000;

    return ret;
}
