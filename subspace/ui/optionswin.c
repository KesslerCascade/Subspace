// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "optionswin.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "ui/page/general/optionsgeneral.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

static void OoptionsWin_registerPages(OptionsWin* self, SubspaceUI* ui)
{
    OptionsPage* p;

    p = OptionsPage(generalpageCreate(ui));
    saPushC(&self->pages, object, &p);
}

_objfactory_guaranteed OptionsWin* OptionsWin_create(SubspaceUI* ui)
{
    OptionsWin* self;
    self = objInstCreate(OptionsWin);

    self->ss = ui->ss;
    self->ui = ui;

    objInstInit(self);
    OoptionsWin_registerPages(self, ui);
    return self;
}

_objinit_guaranteed bool OptionsWin_init(_In_ OptionsWin* self)
{
    // Autogen begins -----
    saInit(&self->pages, object, 1);
    return true;
    // Autogen ends -------
}

static int closebtn_action(Ihandle* ih)
{
    OptionsWin* win = iupGetParentObj(OptionsWin, ih);
    if (win) {
        optionswinOnClose(win->win);
    }
    return IUP_DEFAULT;
}

bool OptionsWin_make(_In_ OptionsWin* self)
{
    if (self->win)
        return true;

    self->pagelist = IupFlatList();
    IupSetAttribute(self->pagelist, "SIZE", "110x30");
    IupSetAttribute(self->pagelist, "EXPAND", "VERTICAL");
    IupSetAttribute(self->pagelist, "FLATSCROLLBAR", "VERTICAL");
    IupSetAttribute(self->pagelist, "ICONSPACING", "4");

    Ihandle* closebtn = IupButton(langGetC(self->ss, _S"options_close"), NULL);
    IupSetAttribute(closebtn, "CPADDING", "6x3");
    IupSetCallback(closebtn, "ACTION", (Icallback)closebtn_action);
    iupSetObj(closebtn, ObjNone, self, self->ui);

    self->pagezbox = IupZbox(NULL);
    IupSetAttribute(self->pagezbox, "CHILDSIZEALL", "NO");

    // set up pages
    for (int i = 0; i < saSize(self->pages); i++) {
        optionspageMake(self->pages.a[i], self->pagelist);
        IupAppend(self->pagezbox, self->pages.a[i]->h);
        IupSetAttributeId(self->pagelist, "", i + 1, strC(self->pages.a[i]->title));
        if (!strEmpty(self->pages.a[i]->imgname))
            IupSetAttributeId(self->pagelist, "IMAGE", i + 1, strC(self->pages.a[i]->imgname));
    }

    Ihandle* pageframe = IupFrame(self->pagezbox);

    Ihandle* dlglayout = IupVbox(IupHbox(self->pagelist, pageframe, NULL),
                                 IupHbox(IupFill(), closebtn, NULL),
                                 NULL);
    IupSetAttribute(dlglayout, "CMARGIN", "3x3");
    IupSetAttribute(dlglayout, "CGAP", "2");

    self->win  = IupDialog(dlglayout);
    string tmp = 0;
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, OPTIONSWIN_WIDTH), stvar(int32, OPTIONSWIN_HEIGHT));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, _S"options_title"));
    iupSetObj(self->win, self, ObjNone, self->ui);
    IupSetAttribute(self->win, "PARENTDIALOG", "SUBSPACE_MAINWIN");

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)OptionsWin_onClose);

    strDestroy(&tmp);
    return true;
}

void OptionsWin_show(_In_ OptionsWin* self)
{
    optionswinMake(self);

    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    IupSetAttribute(self->win, "SIMULATEMODAL", "YES");
    char* rsz = IupGetAttribute(self->win, "RASTERSIZE");
    IupSetAttribute(self->win, "MINSIZE", rsz);

    optionswinUpdateAll(self);
    optionswinShowPage(self, 0);
}

bool OptionsWin_updatePage(_In_ OptionsWin* self, _In_opt_ strref name)
{
    foreach (sarray, idx, OptionsPage*, page, self->pages) {
        if (strEq(name, page->name)) {
            return optionspageUpdate(page);
        }
    }

    return false;
}

void OptionsWin_updateAll(_In_ OptionsWin* self)
{
    foreach (sarray, idx, OptionsPage*, page, self->pages) {
        optionspageUpdate(page);
    }
}

int OptionsWin_onClose(Ihandle* ih)
{
    IupSetAttribute(ih, "SIMULATEMODAL", "NO");
    IupHide(ih);
    return IUP_DEFAULT;
}

void OptionsWin_finish(_In_ OptionsWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    self->win = NULL;
}

void OptionsWin_showPage(_In_ OptionsWin* self, int num)
{
    if (num < 0 || num >= saSize(self->pages))
        return;

    IupSetInt(self->pagelist, "VALUE", num + 1);
    IupSetAttributeHandle(self->pagezbox, "VALUE", self->pages.a[num]->h);
}

void OptionsWin_destroy(_In_ OptionsWin* self)
{
    OptionsWin_finish(self);
    // Autogen begins -----
    saDestroy(&self->pages);
    // Autogen ends -------
}

// Autogen begins -----
#include "optionswin.auto.inc"
// Autogen ends -------
