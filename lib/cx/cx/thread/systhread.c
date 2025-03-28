#include "thread.h"
#include "event.h"
#include "mutex.h"
#include <cx/utils/lazyinit.h>
#include <cx/container/sarray.h>
#include <cx/container/foreach.h>

static Mutex systhreadLock;
static sa_Thread systhreads;
static LazyInitState systhreadInitState;

static void systhreadAtExit(void)
{
    static bool once = true;

    if (!once)
        return;

    once = false;

    mutexAcquire(&systhreadLock);
    // This will wait up to 30 seconds for threads to terminate
    thrShutdownMany(systhreads);
    saDestroy(&systhreads);
    mutexDestroy(&systhreadLock);
}

static void systhreadInit(void *unused)
{
    mutexInit(&systhreadLock);
    saInit(&systhreads, object, 8);
    atexit(systhreadAtExit);
}

_Use_decl_annotations_
void thrRegisterSysThread(Thread *thread)
{
    lazyInit(&systhreadInitState, systhreadInit, NULL);

    if (!thread)
        return;

    withMutex(&systhreadLock) {
        saPush(&systhreads, object, thread);
    }
}
