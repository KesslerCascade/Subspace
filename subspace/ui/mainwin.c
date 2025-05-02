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
#include <cx/format.h>
#include "panel/gameinfo/gameinfopanel.h"
#include "panel/welcome/welcomepanel.h"
#include "ui/util/iuploadimage.h"
#include "ui/util/iupsetobj.h"
#include "optionswin.h"
#include "subspaceui.h"

static void MainWin_registerPanels(MainWin* self, SubspaceUI* ui)
{
    Panel* p;

    p = Panel(gameinfopanelCreate(ui));
    htInsertC(&self->panels, strref, p->name, object, &p);

    self->welcomepanel = Panel(welcomepanelCreate(ui));
}

_objfactory_guaranteed MainWin* MainWin_create(SubspaceUI* ui)
{
    MainWin* self;
    self = objInstCreate(MainWin);

    self->ui = ui;
    self->ss = ui->ss;

    objInstInit(self);
    MainWin_registerPanels(self, self->ui);

    return self;
}

_objinit_guaranteed bool MainWin_init(_In_ MainWin* self)
{
    // Autogen begins -----
    htInit(&self->panels, string, object, 16);
    return true;
    // Autogen ends -------
}

static int optionsbtn_action(Ihandle* ih)
{
    SubspaceUI* ui = iupGetUI(ih);
    if (!ui)
        return IUP_IGNORE;

    optionswinShow(ui->options);
    return IUP_DEFAULT;
}

bool MainWin_make(_In_ MainWin* self)
{
    self->timer = IupTimer();
    iupSetObj(self->timer, ObjNone, self, self->ui);
    IupSetCallback(self->timer, "ACTION_CB", MainWin_onTimer);
    IupSetAttribute(self->timer, "TIME", "10000");
    IupSetAttribute(self->timer, "RUN", "YES");

    foreach (hashtable, hti, self->panels) {
        Panel* p = objDynCast(Panel, htiVal(object, hti));
        if (p)
            panelMake(p);
    }
    panelMake(self->welcomepanel);

    Ihandle* hamburger = IupFlatButton(NULL);
    IupSetAttribute(hamburger, "IMAGE", "IMAGE_HAMBURGER");
    IupSetAttribute(hamburger, "IMAGEHIGHLIGHT", "IMAGE_HAMBURGER_HOVER");
    IupSetAttribute(hamburger, "HLCOLOR", NULL);
    IupSetAttribute(hamburger, "PSCOLOR", NULL);
    IupSetAttribute(hamburger, "BORDERWIDTH", "0");
    IupSetAttribute(hamburger, "TIP", langGetC(self->ss, _S"hamburger_tip"));
    iupSetObj(hamburger, ObjNone, self, self->ui);
    iupLoadImage(self->ss, _S"IMAGE_HAMBURGER", _S"svg", _S"subspace:/hamburger.svg", hamburger);
    iupLoadImage(self->ss, _S"IMAGE_HAMBURGER_HOVER", _S"svg", _S"subspace:/hamburger-hover.svg", NULL);
    Ihandle* options = IupFlatButton(NULL);
    IupSetAttribute(options, "IMAGE", "IMAGE_OPTIONS");
    IupSetAttribute(options, "IMAGEHIGHLIGHT", "IMAGE_OPTIONS_HOVER");
    IupSetAttribute(options, "HLCOLOR", NULL);
    IupSetAttribute(options, "PSCOLOR", NULL);
    IupSetAttribute(options, "BORDERWIDTH", "0");
    IupSetAttribute(options, "TIP", langGetC(self->ss, _S"options_tip"));
    iupSetObj(options, ObjNone, self, self->ui);
    IupSetCallback(options, "FLAT_ACTION", optionsbtn_action);
    iupLoadImage(self->ss, _S"IMAGE_OPTIONS", _S"svg", _S"subspace:/options.svg", options);
    iupLoadImage(self->ss, _S"IMAGE_OPTIONS_HOVER", _S"svg", _S"subspace:/options-hover.svg", NULL);

    self->sidebar = IupVbox(hamburger, options, NULL);
    IupSetAttribute(self->sidebar, "CGAP", "2");
    IupSetAttribute(self->sidebar, "NCMARGIN", "2x2");

    Ihandle* sep = IupFlatSeparator();
    self->zbox   = IupZbox(self->welcomepanel->h);
    mainwinLoadLayout(self);
    IupAppend(self->zbox, self->root);

    string tmp = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/exe", &tmp)) {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->root);
    } else {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->welcomepanel->h);
    }

    self->win = IupDialog(IupHbox(self->sidebar, sep, self->zbox));
    IupSetAttribute(self->win, "MINSIZE", "500x300");   // pixels, not the same units as SIZE
    self->width  = ssdVal(int32, self->ss->settings, _S"ui/size/width", MAINWIN_DEFAULT_WIDTH);
    self->height = ssdVal(int32, self->ss->settings, _S"ui/size/height", MAINWIN_DEFAULT_HEIGHT);
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, self->width), stvar(int32, self->height));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "SHRINK", "Yes");
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, _S"subspace_title"));
    iupSetObj(self->win, self, ObjNone, self->ui);

    IupSetHandle("SUBSPACE_MAINWIN", self->win);

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)MainWin_onClose);
    IupSetCallback(self->win, "RESIZE_CB", (Icallback)MainWin_onResize);

    strDestroy(&tmp);
    return true;
}

void MainWin_show(_In_ MainWin* self)
{
    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    mainwinUpdate(self);
}

void MainWin_update(_In_ MainWin* self)
{
    string tmp = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/exe", &tmp)) {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->root);
    } else {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->welcomepanel->h);
    }

    return;
}

bool MainWin_updatePanel(_In_ MainWin* self, _In_opt_ strref name)
{
    Panel* panel;
    if (htFind(self->panels, strref, name, object, &panel)) {
        panelUpdate(panel);
        objRelease(&panel);
        return true;
    }

    return false;
}

void MainWin_finish(_In_ MainWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    if (self->timer)
        IupDestroy(self->timer);
    self->win   = NULL;
    self->timer = NULL;
}

void MainWin_destroy(_In_ MainWin* self)
{
    MainWin_finish(self);
    // Autogen begins -----
    htDestroy(&self->panels);
    objRelease(&self->welcomepanel);
    // Autogen ends -------
}

static void checkLayout(MainWin* self)
{
    if (self->layoutDirty) {
        self->layoutDirty = false;
        mainwinSaveLayout(self);
    }
}

int MainWin_onClose(Ihandle* ih)
{
    MainWin* self = iupGetObj(MainWin, ih);
    checkLayout(self);
    self->ss->exit = true;
    eventSignal(&self->ss->notify);

    return IUP_DEFAULT;
}

int MainWin_onResize(Ihandle* ih, int width, int height)
{
    if (IupGetInt(ih, "MAXIMIZED") > 0 || IupGetInt(ih, "MINIMIZED") > 0)
        return IUP_DEFAULT;   // don't save maximized "size"

    MainWin* self = iupGetObj(MainWin, ih);
    int w, h;
    if (IupGetIntInt(ih, "SIZE", &w, &h) == 2 && (w != self->width || h != self->height)) {
        self->width  = w;
        self->height = h;
        mainwinSetLayoutDirty(self);
    }

    return IUP_DEFAULT;
}

int MainWin_onTimer(Ihandle* ih)
{
    MainWin* self = iupGetParentObj(MainWin, ih);
    if (self)
        checkLayout(self);
    return IUP_DEFAULT;
}

void MainWin_setLayoutDirty(_In_ MainWin* self)
{
    self->layoutDirty = true;
}

// Autogen begins -----
void MainWin_loadLayout(_In_ MainWin* self);
void MainWin_saveLayout(_In_ MainWin* self);
Ihandle* MainWin_createPlaceholder(_In_ MainWin* self);
Ihandle* MainWin_createTabs(_In_ MainWin* self);
Ihandle* MainWin_createSplit(_In_ MainWin* self, bool vertical);
void MainWin_replaceSplitChild(_In_ MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh);
#include "mainwin.auto.inc"
// Autogen ends -------
