#include "cx/platform/os.h"
#include "cx/utils/lazyinit.h"
#include "cx/time/time.h"
#include <pthread.h>
#include <time.h>

#if defined(_PLATFORM_FBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(_PLATFORM_LINUX)
#include <unistd.h>
#include <sched.h>
#endif

static LazyInitState coreCache;
static int ncores;
static int nlogical;

void osYield()
{
    sched_yield();
}

void osSleep(int64 time)
{
    struct timespec ts;
    timeToRelTimespec(&ts, time);
    while(nanosleep(&ts, &ts)) {}
}

static void initCoreCache(void *dummy)
{
#ifdef _PLATFORM_FBSD
    int mib[2];
    size_t len = sizeof(nlogical);

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;

    sysctl(mib, 2, &nlogical, &len, NULL, 0);

    ncores = nlogical;      // TODO: figure out a way to get physical core count that
                            // doesn't involve parsing XML...
#elif _PLATFORM_LINUX
    nlogical = sysconf(_SC_NPROCESSORS_ONLN);

    ncores = nlogical;      // TODO: get physical core count
#endif

    // fallback to prevent bad things like dividing by zero
    if (nlogical < 1)
        nlogical = 1;
    if (ncores < 1)
        ncores = 1;
}

int osPhysicalCPUs()
{
    lazyInit(&coreCache, initCoreCache, NULL);
    return ncores;
}

int osLogicalCPUs()
{
    lazyInit(&coreCache, initCoreCache, NULL);
    return nlogical;
}
