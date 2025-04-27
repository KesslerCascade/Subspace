// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "mainwin.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/util/iuploadimage.h"
#include "subspaceui.h"

_objfactory_guaranteed MainWin* MainWin_create(SubspaceUI* ui)
{
    MainWin* self;
    self = objInstCreate(MainWin);

    self->ui = objGetWeak(SubspaceUI, ui);
    self->ss = ui->ss;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool MainWin_init(_In_ MainWin* self)
{
    Ihandle* label1 = IupLabel(langGetC(self->ss, _S"ftl"));
    Ihandle* label2 = IupLabel(langGetC(self->ss, _S"fltl"));
    Ihandle* label3 = IupLabel("shark");
    Ihandle* label4 = IupLabel("tiger");
    Ihandle* label5 = IupLabel("underbelly");
    IupSetAttribute(label3, "IMAGE", "dummy");
    IupSetAttribute(label4, "IMAGE", "dummy");
    Ihandle* vbox = IupVbox(label1, label2, label3, label4, label5, NULL);
    self->win     = IupDialog(vbox);
    IupSetAttribute(self->win, "SIZE", "300x300");
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, _S"subspace_title"));
    IupSetAttribute(self->win, "CXOBJ", (const char*)self);

    iupLoadImage(self->ss->ui, label4, _S"IMAGE", _S"svg", _S"subspace:/23.svg");
    iupLoadImage(self->ss->ui, label3, _S"IMAGE", _S"png", _S"subspace:/shark.png");

    IupSetCallback(self->win, "CLOSE_CB", MainWin_onClose);

    // Autogen begins -----
    return true;
    // Autogen ends -------
}

void MainWin_show(_In_ MainWin* self)
{
    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
}

void MainWin_update(_In_ MainWin* self)
{
    // nothing yet
}

void MainWin_destroy(_In_ MainWin* self)
{
    IupDestroy(self->win);
    // Autogen begins -----
    objDestroyWeak(&self->ui);
    // Autogen ends -------
}

int MainWin_onClose(Ihandle* iupself)
{
    MainWin* self  = (MainWin*)IupGetAttribute(iupself, "CXOBJ");
    self->ss->exit = true;
    eventSignal(&self->ss->notify);

    return IUP_DEFAULT;
}

// Autogen begins -----
#include "mainwin.auto.inc"
// Autogen ends -------
