#include "settings_private.h"
#include <cx/thread.h>
#include <cx/utils.h>

static Mutex setsthreadlock;
static sa_SSDNode setslist;

static LazyInitState setsthread_initstate;

static int setsthread_func(Thread *self)
{
    sa_SSDNode toprocess;
    saInit(&toprocess, object, 8);

    thrRegisterSysThread(self);

    while (thrLoop(self)) {
        int64 now = clockTimer();
        int64 nextcheck = now + SETTINGS_DEFAULT_FLUSH_INTERVAL;

        // with the lock held, acquire object references to ensure
        // the objects survive during processing
        mutexAcquire(&setsthreadlock);
        foreach(sarray, idx, SSDNode*, cur, setslist)
        {
            SettingsTree *tree = objDynCast(SettingsTree, cur->tree);
            if (!tree)
                continue;

            if (tree->check + tree->interval < now) {
                tree->check = now;
                saPush(&toprocess, object, cur);
            }
            nextcheck = min(nextcheck, tree->check + tree->interval);
        }
        mutexRelease(&setsthreadlock);

        // go through all the settings that need to be checked
        foreach(sarray, idx, SSDNode*, cur, toprocess)
        {
            setsFlush(cur);
        }

        saClear(&toprocess);

        eventWaitTimeout(&self->notify, max(nextcheck - now, timeS(1)));
    }
    return 0;
}

static void setsthread_init(void *unused)
{
    mutexInit(&setsthreadlock);
    saInit(&setslist, ptr, 16, SA_Sorted);
    thrRun(setsthread_func, _S"CX Settings Writer", stvNone);
}

void _setsThreadCheck(void)
{
    lazyInit(&setsthread_initstate, setsthread_init, NULL);
}

void _setsThreadWatch(SSDNode *sets)
{
    SSDNode *o = objAcquire(sets);
    withMutex(&setsthreadlock)
    {
        saPush(&setslist, ptr, o);
    }
}

void _setsThreadForget(SSDNode *sets)
{
    withMutex(&setsthreadlock)
    {
        if (saFindRemove(&setslist, ptr, sets))
            objRelease(&sets);
    }
}
