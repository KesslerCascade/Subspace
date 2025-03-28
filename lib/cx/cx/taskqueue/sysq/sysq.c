#include "sysq.h"
#include <cx/utils/lazyinit.h>

static TaskQueue *sysq;

static void sysqExit(void)
{
    tqShutdown(sysq, timeS(30));
    tqRelease(&sysq);
}

static LazyInitState sysqInitState;
static void sysqInitFunc(void *dummy)
{
    TaskQueueConfig conf;
    tqPresetBalanced(&conf);
    conf.pool.wIdle = 1;        // use only 1 thread for idle
    sysq                 = tqCreate(_S"CX System", &conf);
    relAssertMsg(sysq, "Failed to create system queue");
    atexit(sysqExit);
}

static _meta_inline void sysqInit(void)
{
    lazyInit(&sysqInitState, sysqInitFunc, NULL);
}

_Use_decl_annotations_
bool _sysqAdd(BasicTask* task)
{
    sysqInit();
    return tqAdd(sysq, task);
}

_Use_decl_annotations_
bool _sysqSchedule(ComplexTask* task, int64 delay)
{
    sysqInit();
    return tqSchedule(sysq, task, delay);
}

_Use_decl_annotations_
bool _sysqDefer(ComplexTask* task)
{
    sysqInit();
    return tqDefer(sysq, task);
}

_Use_decl_annotations_
bool sysqCall(UserTaskCB func, void* userdata)
{
    sysqInit();
    return tqCall(sysq, func, userdata);
}
