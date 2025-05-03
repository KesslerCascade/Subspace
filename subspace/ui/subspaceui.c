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

#include "ui/util/uiupdatedispatch.h"
#include "mainwin.h"
#include "optionswin.h"

#include <cd.h>
#include <iup.h>
#include <iup_plot.h>
#include <iupcontrols.h>

#ifdef WIN32
#include <windows.h>
#endif

static bool uicb(TaskQueue* tq)
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
    IupSetGlobal("UIOBJ", (char*)ui);

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
    self->uiq    = tqCreate(_S"UI", &conf);
    if (!self->uiq || !tqStart(self->uiq))
        return false;

    // Some initialization must be performed in the UI thread itself
    SubspaceUI*
        uithreadref = objAcquire(self);   // the UI thread owns a reference to ensure that the UI
                                          // object isn't destroyed so long as it's running
    tqCall(self->uiq, uiThreadInit, (void*)uithreadref);

    return true;
}

static bool uiThreadShutdown(TaskQueue* tq, void* data)
{
    SharedEvent* e = (SharedEvent*)data;
    SubspaceUI* ui = (SubspaceUI*)IupGetGlobal("UIOBJ");
    IupClose();

    // ok for UI object to be destroyed now
    objRelease(&ui);

    eventSignal(&e->ev);
    sheventRelease(&e);

    return true;
}

bool SubspaceUI_shutdown(_In_ SubspaceUI* self)
{
    if (!self->uiq)
        return false;
    ssuiStop(self);

    SharedEvent* callEvent = sheventCreate(0);

    // Tear down UI from inside the UI thread itself
    tqCall(self->uiq, uiThreadShutdown, (void*)sheventAcquire(callEvent));
    eventWaitTimeout(&callEvent->ev, timeS(10));

    tqShutdown(self->uiq, true);
    sheventRelease(&callEvent);

    return true;
}

static bool uiStartFunc(TaskQueue* tq, void* data)
{
    SubspaceUI* ui = (SubspaceUI*)IupGetGlobal("UIOBJ");
    if (!ui)
        return false;

    ui->main = mainwinCreate(ui);
    if (!mainwinMake(ui->main))
        return false;
    ui->options = optionswinCreate(ui);
    mainwinShow(ui->main);

    return true;
}

void SubspaceUI_start(_In_ SubspaceUI* self)
{
    if (self->started)
        return;

    // load language translations
    string lang = 0;
    ssdStringOutD(self->ss->settings, _S"ui/lang", &lang, _S"en-us");
    if (!langLoad(self->ss, lang)) {
        fatalError(_S"Could not load any UI language.", false);
    }
    strDestroy(&lang);

    // Need to run the function to create the UI from the UI thread itself
    tqCall(self->uiq, uiStartFunc, NULL);
    self->started = true;
}

static bool uiStopFunc(TaskQueue* tq, void* data)
{
    SubspaceUI* ui = (SubspaceUI*)IupGetGlobal("UIOBJ");
    if (!ui)
        return false;

    optionswinFinish(ui->options);
    mainwinFinish(ui->main);
    objRelease(&ui->options);
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

    objRelease(&self->ss->lang);
    strDestroy(&self->ss->langid);
}

void SubspaceUI_destroy(_In_ SubspaceUI* self)
{
    // Autogen begins -----
    objRelease(&self->uiq);
    objRelease(&self->main);
    objRelease(&self->options);
    // Autogen ends -------
}

void SubspaceUI_update(_In_ SubspaceUI* self)
{
    UIUpdateDispatch* disp = uiupdatedispatchAll(self);
    tqRun(self->uiq, &disp);
}

void SubspaceUI_updateMain(_In_ SubspaceUI* self, _In_opt_ strref panel)
{
    UIUpdateDispatch* disp = uiupdatedispatchMainWin(self, panel);
    tqRun(self->uiq, &disp);
}

void SubspaceUI_updateOptions(_In_ SubspaceUI* self, _In_opt_ strref page)
{
    UIUpdateDispatch* disp = uiupdatedispatchOptions(self, page);
    tqRun(self->uiq, &disp);
}

// Autogen begins -----
#include "subspaceui.auto.inc"
// Autogen ends -------
