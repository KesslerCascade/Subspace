#include "uithread.h"

#include "subspace.h"

#include <cd.h>
#include <iup.h>
#include <iup_plot.h>
#include <iupcontrols.h>

#ifdef WIN32
#include <windows.h>
#endif

static bool uicb(TaskQueue *tq)
{
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

    IupControlsOpen();
    IupPlotOpen();

    return true;
}

bool uiInit(UIThread* uithr, Subspace *ss)
{
    uithr->ss = ss;

    // ui queue is a special task queue with only a single worker thread
    TaskQueueConfig conf;
    tqPresetSingle(&conf);
    conf.pool.ui = uicb;
    uithr->uiq   = tqCreate(_S"UI", &conf);
    if (!uithr->uiq || !tqStart(uithr->uiq))
        return false;

    // it also gets some worker threads for tasks like decoding images
    tqPresetBalanced(&conf);
    uithr->uiworkers = tqCreate(_S"UIWorkers", &conf);
    if (!uithr->uiworkers || !tqStart(uithr->uiworkers))
        return false;

    // Some initialization must be performed in the UI thread itself
    tqCall(uithr->uiq, uiThreadInit, (void*)uithr);

    return true;
}

static bool uiStartFunc(TaskQueue* tq, void* data)
{
    UIThread* uithr = (UIThread*)data;
    IupMessage("Hello World 1", "Hello world from IUP.");
    uithr->ss->exit = true;
    eventSignal(&uithr->ss->notify);

    return true;
}

void uiStart(UIThread* uithr)
{
    if (uithr->started)
        return;

    // Need to run the function to create the UI from the UI thread itself
    tqCall(uithr->uiq, uiStartFunc, uithr);
    uithr->started = true;
}

static bool uiStopFunc(TaskQueue* tq, void* data)
{
    UIThread* uithr = (UIThread*)data;
    uithr->ss->exit = true;
    eventSignal(&uithr->ss->notify);

    return true;
}

bool uiStop(UIThread* uithr)
{
    if (!uithr->started)
        return false;

    // Notify the UI thread that it should start shutting things down
    tqCall(uithr->uiq, uiStopFunc, uithr);
    uithr->started = false;

    return true;
}

typedef struct UIThreadShutdownData {
    UIThread* uithr;
    SharedEvent* e;
} UIThreadShutdownData;
static bool uiThreadShutdown(TaskQueue* tq, void* data)
{
    UIThreadShutdownData* d = (UIThreadShutdownData*)data;
    IupClose();

    eventSignal(&d->e->ev);
    sheventRelease(&d->e);

    xaFree(d);
    return true;
}

bool uiShutdown(UIThread* uithr)
{
    if (!uithr->uiq)
        return false;
    uiStop(uithr);

    UIThreadShutdownData* sd = xaAllocStruct(UIThreadShutdownData, XA_Zero);
    SharedEvent *callEvent = sheventCreate(0);

    // Tear down UI from inside the UI thread itself
    sd->e     = sheventAcquire(callEvent);
    sd->uithr = uithr;
    tqCall(uithr->uiq, uiThreadShutdown, sd);
    eventWaitTimeout(&callEvent->ev, timeS(10));

    tqShutdown(uithr->uiworkers, true);
    tqShutdown(uithr->uiq, true);
    objRelease(&uithr->uiworkers);
    objRelease(&uithr->uiq);
    sheventRelease(&callEvent);

    return true;
}