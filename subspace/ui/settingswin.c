// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "settingswin.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "feature/featureregistry.h"
#include "ui/page/features/featurespage.h"
#include "ui/page/setup/setuppage.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

static void SettingsWin_registerPages(SettingsWin* self, SubspaceUI* ui)
{
    SettingsPage* p;

    p = SettingsPage(setuppageCreate(ui));
    saPushC(&self->pages, object, &p);
    p = SettingsPage(featurespageCreate(ui));
    saPushC(&self->pages, object, &p);

    // register pages that belong to features
    foreach (hashtable, hti, self->ss->freg->features) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(object, hti);
        SettingsPage* page    = featureGetSettingsPage(feat);
        if (page) {
            saPush(&self->pages, object, page);
        }
    }
}

_objfactory_guaranteed SettingsWin* SettingsWin_create(SubspaceUI* ui)
{
    SettingsWin* self;
    self = objInstCreate(SettingsWin);

    self->ss = ui->ss;
    self->ui = ui;

    objInstInit(self);
    SettingsWin_registerPages(self, ui);
    return self;
}

_objinit_guaranteed bool SettingsWin_init(_In_ SettingsWin* self)
{
    // Autogen begins -----
    saInit(&self->pages, object, 1);
    saInit(&self->zboxmap, int32, 1);
    return true;
    // Autogen ends -------
}

static int closebtn_action(Ihandle* ih)
{
    SettingsWin* win = iupGetParentObj(SettingsWin, ih);
    if (win) {
        settingswinOnClose(win->win);
    }
    return IUP_DEFAULT;
}

static int pagelist_change(Ihandle* ih, char* text, int item, int state)
{
    SettingsWin* win = iupGetParentObj(SettingsWin, ih);
    if (win) {
        settingswinShowPageByList(win, item);
    }
    return IUP_DEFAULT;
}

bool SettingsWin_make(_In_ SettingsWin* self)
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

    Ihandle* closebtn = IupButton(langGetC(self->ss, _S"settings_close"), NULL);
    IupSetAttribute(closebtn, "CPADDING", "6x3");
    IupSetCallback(closebtn, "ACTION", (Icallback)closebtn_action);
    iupSetObj(closebtn, ObjNone, self, self->ui);

    self->pagezbox = IupZbox(NULL);
    IupSetAttribute(self->pagezbox, "CHILDSIZEALL", "NO");

    Ihandle* pageframe = IupFrame(self->pagezbox);

    // set up pages
    for (int i = 0; i < saSize(self->pages); i++) {
        settingspageMake(self->pages.a[i], self->pagelist);
        IupAppend(self->pagezbox, self->pages.a[i]->h);
    }

    Ihandle* dlglayout = IupVbox(IupHbox(self->pagelist, pageframe, NULL),
                                 IupHbox(IupFill(), closebtn, NULL),
                                 NULL);
    IupSetAttribute(dlglayout, "CMARGIN", "3x3");
    IupSetAttribute(dlglayout, "CGAP", "2");

    self->win  = IupDialog(dlglayout);
    string tmp = 0;
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, SETTINGSWIN_WIDTH), stvar(int32, SETTINGSWIN_HEIGHT));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "RESIZE", "NO");
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, _S"settings_title"));
    iupSetObj(self->win, self, ObjNone, self->ui);
    IupSetAttribute(self->win, "PARENTDIALOG", "SUBSPACE_MAINWIN");

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)SettingsWin_onClose);

    strDestroy(&tmp);
    return true;
}

void SettingsWin_show(_In_ SettingsWin* self)
{
    settingswinMake(self);

    IupMap(self->win);
    settingswinUpdateList(self);
    settingswinUpdateAll(self);

    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    IupSetAttribute(self->win, "SIMULATEMODAL", "YES");

    settingswinShowPage(self, 0);

    // queue up a redraw after the window is shown; this fixes a 1 pixel shift when it's first
    // updated
    ssuiUpdateSettings(self->ui, NULL);
}

bool SettingsWin_updatePage(_In_ SettingsWin* self, _In_opt_ strref name)
{
    foreach (sarray, idx, SettingsPage*, page, self->pages) {
        if (strEq(name, page->name)) {
            return settingspageUpdate(page);
        }
    }

    return false;
}

void SettingsWin_updateAll(_In_ SettingsWin* self)
{
    settingswinUpdateList(self);
    foreach (sarray, idx, SettingsPage*, page, self->pages) {
        settingspageUpdate(page);
    }
}

int SettingsWin_onClose(Ihandle* ih)
{
    IupSetAttribute(ih, "SIMULATEMODAL", "NO");
    IupHide(ih);
    return IUP_DEFAULT;
}

void SettingsWin_finish(_In_ SettingsWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    self->win = NULL;
}

void SettingsWin_showPage(_In_ SettingsWin* self, int num)
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

void SettingsWin_showPageByList(_In_ SettingsWin* self, int listid)
{
    if (listid < 1 || listid > saSize(self->zboxmap))
        return;

    IupSetInt(self->pagelist, "VALUE", listid);
    IupSetAttributeHandle(self->pagezbox, "VALUE", self->pages.a[self->zboxmap.a[listid - 1]]->h);
}

void SettingsWin_updateList(_In_ SettingsWin* self)
{
    int curval           = IupGetInt(self->pagelist, "VALUE");
    string curtitle      = 0;

    if (curval > 0)
        strDup(&curtitle, (strref)IupGetAttributeId(self->pagelist, "", curval));

    // clear list
    IupSetAttribute(self->pagelist, "1", NULL);
    saClear(&self->zboxmap);

    // build out the list
    int listid = 1;
    for (int i = 0; i < saSize(self->pages); i++) {
        SettingsPage* page = self->pages.a[i];
        bool visible       = true;

        // if this page is linked to a feature, it must be enabled tobe visible
        SubspaceFeature* feat = objAcquireFromWeak(SubspaceFeature, page->owner);
        if (feat)
            visible &= featureIsEnabled(feat);

        objRelease(&feat);

        if (visible) {
            IupSetAttributeId(self->pagelist, "", listid, strC(page->title));

            if (curtitle && strEq(curtitle, page->title))
                IupSetInt(self->pagelist, "VALUE", listid);   // restore current selection

            if (!strEmpty(page->imgname))
                IupSetAttributeId(self->pagelist, "IMAGE", listid, strC(page->imgname));

            // fill out array of actual page indices in list ID order
            saPush(&self->zboxmap, int32, i);

            listid++;
        }
    }

    strDestroy(&curtitle);
}

void SettingsWin_destroy(_In_ SettingsWin* self)
{
    SettingsWin_finish(self);
    // Autogen begins -----
    saDestroy(&self->pages);
    saDestroy(&self->zboxmap);
    // Autogen ends -------
}

// Autogen begins -----
#include "settingswin.auto.inc"
// Autogen ends -------
