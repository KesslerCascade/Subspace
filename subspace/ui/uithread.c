#include "uithread.h"

#include "lua/task/luacall.h"
#include "lua/task/luaexec.h"
#include "subspace.h"

#include <iup.h>
#include <iuplua.h>

#ifdef WIN32
#include <windows.h>
#endif

static atomic(bool) uirunning;

static bool uicb(TaskQueue *tq)
{
        // failsafe as lua still has some tasks to process even after the UI is shut down
    if (!atomicLoad(bool, &uirunning, Acquire))
        return true;

    IupLoopStep();

    #ifdef WIN32
    MSG msg;
    if (PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
        return false;   // more messages to process
    #endif

    return true;

}

static bool uiThreadInit(TaskQueue* tq, void* data)
{
    IupOpen(NULL, NULL);
    IupSetGlobal("LOCKLOOP", "YES");
    IupSetGlobal("UTF8MODE", "YES");
    IupSetGlobal("UTF8MODE_FILE", "YES");

    UIThread* uithr = (UIThread*)data;
    iuplua_open(uithr->lua.L);
    atomicStore(bool, &uirunning, true, Release);
    return true;
}

static bool uiThreadShutdown(TaskQueue* tq, void* data)
{
    atomicStore(bool, &uirunning, false, Release);
#ifdef WITH_GUI
    IupClose();
#endif

    SharedEvent* e = (SharedEvent*)data;
    eventSignal(&e->ev);
    sheventRelease(&e);
    return true;
}

bool uiLoadLua(UIThread *uithr, strref filename)
{
    bool ret         = false;

    logFmt(Verbose, _S"ui: Loading lua script ${string}", stvar(strref, filename));
    LuaExecFile* task = luaexecfileCreate(&uithr->lua, filename);
    luaAddTask(&uithr->lua, task);

    taskWait(task, timeForever);

    ret = taskSucceeded(task);
    objRelease(&task);
    return ret;
}

bool uiInit(UIThread* uithr, Subspace *ss)
{
    uithr->ss = ss;

    // ui queue is a special task queue with only a single worker thread
    TaskQueueConfig conf;
    tqPresetSingle(&conf);
    conf.pool.ui = uicb;
    uithr->uiq  = tqCreate(_S"LuaUI", &conf);
    if (!uithr->uiq || !tqStart(uithr->uiq))
        return false;

    // it also gets some worker threads for tasks like decoding images
    tqPresetBalanced(&conf);
    uithr->uiworkers = tqCreate(_S"UIWorkers", &conf);
    if (!uithr->uiworkers || !tqStart(uithr->uiworkers))
        return false;

    // lua needs a reference to our queues and structure
    luaInit(&uithr->lua, ss, uithr->uiq, uithr->uiworkers);

    // Some initialization must be performed in the UI thread itself
    tqCall(uithr->uiq, uiThreadInit, (void*)uithr);

    return true;
}

static bool startupCb(stvlist *cvars, stvlist *args)
{
    Task* task = stvlNextObj(args, Task);
    UIThread* uithr = stvlNextPtr(cvars);

    if (!(task && uithr))
        return false;

    if (!taskSucceeded(task)) {
        logStr(Error, _S"uithread: startup failed!");
        uithr->ss->exit = true;
        eventSignal(&uithr->ss->notify);
    }
    return true;
}

void uiStart(UIThread* uithr)
{
    // Notify LUA to start up the UI
    LuaCall* task = luacallCreate(&uithr->lua, _S"startup", true, 0, stvNone);
    cchainAttach(&task->oncomplete, startupCb, stvar(ptr, uithr));
    luaRun(&uithr->lua, task);
    uithr->started = true;
}

bool uiStop(UIThread* uithr)
{
    if (!uithr->started)
        return false;

    // notify LUA environment of the shutdown
    LuaCall* task = luacallCreate(&uithr->lua, _S"shutdown", false, 0, stvNone);
    luaAddTask(&uithr->lua, task);

    // wait for call to finish
    taskWait(task, timeS(10));
    objRelease(&task);

    uithr->started = false;

    return true;
}

bool uiShutdown(UIThread* uithr)
{
    if (!uithr->uiq)
        return false;
    uiStop(uithr);

    SharedEvent *callEvent = sheventCreate(0);

    // Tear down UI from inside the UI thread itself
    tqCall(uithr->uiq, uiThreadShutdown, sheventAcquire(callEvent));
    eventWaitTimeout(&callEvent->ev, timeS(10));

    tqShutdown(uithr->uiworkers, true);
    tqShutdown(uithr->uiq, true);
    objRelease(&uithr->uiworkers);
    objRelease(&uithr->uiq);
    sheventRelease(&callEvent);

    luaShutdown(&uithr->lua);
    return true;
}