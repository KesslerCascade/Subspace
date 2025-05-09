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
#include "ui/page/features/optionsfeatures.h"
#include "ui/page/setup/optionssetup.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

static void OoptionsWin_registerPages(OptionsWin* self, SubspaceUI* ui)
{
    OptionsPage* p;

    p = OptionsPage(setuppageCreate(ui));
    saPushC(&self->pages, object, &p);
    p = OptionsPage(featurespageCreate(ui));
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
    saInit(&self->zboxmap, int32, 1);
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

static int pagelist_change(Ihandle* ih, char* text, int item, int state)
{
    OptionsWin* win = iupGetParentObj(OptionsWin, ih);
    if (win) {
        optionswinShowPageByList(win, item);
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
    iupSetObj(self->pagelist, ObjNone, self, self->ui);
    IupSetCallback(self->pagelist, "FLAT_ACTION", (Icallback)pagelist_change);

    Ihandle* closebtn = IupButton(langGetC(self->ss, _S"options_close"), NULL);
    IupSetAttribute(closebtn, "CPADDING", "6x3");
    IupSetCallback(closebtn, "ACTION", (Icallback)closebtn_action);
    iupSetObj(closebtn, ObjNone, self, self->ui);

    self->pagezbox = IupZbox(NULL);
    IupSetAttribute(self->pagezbox, "CHILDSIZEALL", "NO");

    Ihandle* pageframe = IupFrame(self->pagezbox);

    // set up pages
    for (int i = 0; i < saSize(self->pages); i++) {
        optionspageMake(self->pages.a[i], self->pagelist);
        IupAppend(self->pagezbox, self->pages.a[i]->h);
    }

    Ihandle* dlglayout = IupVbox(IupHbox(self->pagelist, pageframe, NULL),
                                 IupHbox(IupFill(), closebtn, NULL),
                                 NULL);
    IupSetAttribute(dlglayout, "CMARGIN", "3x3");
    IupSetAttribute(dlglayout, "CGAP", "2");

    self->win  = IupDialog(dlglayout);
    string tmp = 0;
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, OPTIONSWIN_WIDTH), stvar(int32, OPTIONSWIN_HEIGHT));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "RESIZE", "NO");
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

    IupMap(self->win);
    optionswinUpdateList(self);
    optionswinUpdateAll(self);

    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    IupSetAttribute(self->win, "SIMULATEMODAL", "YES");

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

    for (int i = 0; i < saSize(self->zboxmap); i++) {
        if (num == self->zboxmap.a[i]) {
            IupSetInt(self->pagelist, "VALUE", i + 1);
            break;
        }
    }
    IupSetAttributeHandle(self->pagezbox, "VALUE", self->pages.a[num]->h);
}

void OptionsWin_showPageByList(_In_ OptionsWin* self, int listid)
{
    if (listid < 1 || listid > saSize(self->zboxmap))
        return;

    IupSetInt(self->pagelist, "VALUE", listid);
    IupSetAttributeHandle(self->pagezbox, "VALUE", self->pages.a[self->zboxmap.a[listid - 1]]->h);
}

void OptionsWin_updateList(_In_ OptionsWin* self)
{
    int curval           = IupGetInt(self->pagelist, "VALUE");
    const char* curtitle = (curval > 0) ? IupGetAttributeId(self->pagelist, "", curval) : NULL;

    // clear list
    IupSetAttribute(self->pagelist, "1", NULL);
    saClear(&self->zboxmap);

    // build out the list
    int listid = 1;
    for (int i = 0; i < saSize(self->pages); i++) {
        OptionsPage* page = self->pages.a[i];
        if (page->visible) {
            IupSetAttributeId(self->pagelist, "", listid, strC(page->title));

            if (curtitle && strEq((strref)curtitle, page->title))
                IupSetInt(self->pagelist, "VALUE", listid);   // restore current selection

            if (!strEmpty(page->imgname))
                IupSetAttributeId(self->pagelist, "IMAGE", listid, strC(page->imgname));

            // fill out array of actual page indices in list ID order
            saPush(&self->zboxmap, int32, i);

            listid++;
        }
    }
}

void OptionsWin_destroy(_In_ OptionsWin* self)
{
    OptionsWin_finish(self);
    // Autogen begins -----
    saDestroy(&self->pages);
    saDestroy(&self->zboxmap);
    // Autogen ends -------
}

// Autogen begins -----
#include "optionswin.auto.inc"
// Autogen ends -------
