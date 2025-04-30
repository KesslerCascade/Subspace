// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "subspaceui.h"
// clang-format on
// ==================== Auto-generated section ends ======================

#include "mainwin.h"

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
    SubspaceUI* ui = (SubspaceUI*)data;

    IupOpen(NULL, NULL);
    IupSetGlobal("LOCKLOOP", "YES");
    IupSetGlobal("UTF8MODE", "YES");
    IupSetGlobal("UTF8MODE_FILE", "YES");

    if (ui->ss->devmode)
        IupSetGlobal("GLOBALLAYOUTDLGKEY", "Yes");

    IupControlsOpen();
    IupPlotOpen();

    return true;
}

_objfactory_guaranteed SubspaceUI* SubspaceUI_create(Subspace* subspace)
{
    SubspaceUI* self;
    self = objInstCreate(SubspaceUI);

    self->ss = subspace;

    objInstInit(self);
    return self;
}

bool SubspaceUI_init(_In_ SubspaceUI* self)
{
    // ui queue is a special task queue with only a single worker thread
    TaskQueueConfig conf;
    tqPresetSingle(&conf);
    conf.pool.ui = uicb;
    self->uiq   = tqCreate(_S"UI", &conf);
    if (!self->uiq || !tqStart(self->uiq))
        return false;

    // it also gets some worker threads for tasks like decoding images
    tqPresetBalanced(&conf);
    self->uiworkers = tqCreate(_S"UIWorkers", &conf);
    if (!self->uiworkers || !tqStart(self->uiworkers))
        return false;

    // Some initialization must be performed in the UI thread itself
    tqCall(self->uiq, uiThreadInit, (void*)self);

    return true;    
}

typedef struct UIThreadShutdownData {
    SubspaceUI* ui;
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

bool SubspaceUI_shutdown(_In_ SubspaceUI* self)
{
    if (!self->uiq)
        return false;
    ssuiStop(self);

    UIThreadShutdownData* sd = xaAllocStruct(UIThreadShutdownData, XA_Zero);
    SharedEvent *callEvent = sheventCreate(0);

    // Tear down UI from inside the UI thread itself
    sd->e     = sheventAcquire(callEvent);
    sd->ui = self;
    tqCall(self->uiq, uiThreadShutdown, sd);
    eventWaitTimeout(&callEvent->ev, timeS(10));

    tqShutdown(self->uiworkers, true);
    tqShutdown(self->uiq, true);
    sheventRelease(&callEvent);

    return true;
}

static bool uiStartFunc(TaskQueue* tq, void* data)
{
    SubspaceUI* ui = (SubspaceUI*)data;

    ui->main = mainwinCreate(ui);
    if (!mainwinMake(ui->main))
        return false;
    mainwinShow(ui->main);

    return true;
}

void SubspaceUI_start(_In_ SubspaceUI* self)
{
    if (self->started)
        return;

    // Need to run the function to create the UI from the UI thread itself
    tqCall(self->uiq, uiStartFunc, self);
    self->started = true;
}

static bool uiStopFunc(TaskQueue* tq, void* data)
{
    SubspaceUI* ui = (SubspaceUI*)data;
    objRelease(&ui->main);

    return true;
}

void SubspaceUI_stop(_In_ SubspaceUI* self)
{
    if (!self->started)
        return;

    // Notify the UI thread that it should start shutting things down
    tqCall(self->uiq, uiStopFunc, self);
    self->started = false;
}

void SubspaceUI_destroy(_In_ SubspaceUI* self)
{
    // Autogen begins -----
    objRelease(&self->uiq);
    objRelease(&self->uiworkers);
    objRelease(&self->main);
    // Autogen ends -------
}

// Autogen begins -----
#include "subspaceui.auto.inc"
// Autogen ends -------
