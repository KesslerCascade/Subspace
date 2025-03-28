#include "win_time.h"
#include "cx/utils/lazyinit.h"
#include "cx/platform/win.h"

int64 clockWall()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    return timeFromFileTime(&ft);
}

int64 clockWallLocal()
{
    FILETIME ft, lt;
    GetSystemTimeAsFileTime(&ft);
    FileTimeToLocalFileTime(&ft, &lt);
    return timeFromFileTime(&lt);
}

static LazyInitState qpcInitState;
static uint64 qpcMult;
static uint64 qpcDivisor;

static void qpcInit(void *data)
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    if (freq.QuadPart > 1000000) {
        qpcMult = 10;
        qpcDivisor = freq.QuadPart / 100000;
    } else {
        qpcMult = 100000 / freq.QuadPart;
        qpcDivisor = 10;
    }
}

int64 clockTimer()
{
    lazyInit(&qpcInitState, qpcInit, 0);

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);

    return (int64)(counter.QuadPart * qpcMult / qpcDivisor);
}
