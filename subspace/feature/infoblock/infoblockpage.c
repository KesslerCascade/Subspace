// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "infoblockpage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "infoblock.h"

static strref infoitems[]     = { _S"ssver", _S"ftlver", _S"fps" };
static const int numinfoitems = 3;

_objfactory_guaranteed InfoBlockPage* InfoBlockPage_create(InfoBlock* feature, SubspaceUI* ui)
{
    InfoBlockPage* self;
    self = objInstCreate(InfoBlockPage);

    self->feature = feature;
    self->owner   = objGetWeak(SubspaceFeature, feature);
    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"infoblock";
    strDup(&self->title, langGet(self->ss, _S"feature_infoblock"));
    strDup(&self->imgname, _S"IMAGE_INFOBLOCK");

    objInstInit(self);

    return self;
}

static void elist_update(InfoBlockPage* page)
{
    Ihandle* ih = page->enabledlist;
    int val     = IupGetInt(ih, "VALUE");
    int count   = IupGetInt(ih, "COUNT");

    if (val > 0)
        IupSetAttribute(page->removebtn, "ACTIVE", "YES");
    else
        IupSetAttribute(page->removebtn, "ACTIVE", "NO");

    if (val > 1)
        IupSetAttribute(page->upbtn, "ACTIVE", "YES");
    else
        IupSetAttribute(page->upbtn, "ACTIVE", "NO");

    if (val != 0 && val < count)
        IupSetAttribute(page->downbtn, "ACTIVE", "YES");
    else
        IupSetAttribute(page->downbtn, "ACTIVE", "NO");
}

static int elist_action(Ihandle* ih, char* text, int item, int state)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    elist_update(page);

    return IUP_DEFAULT;
}

static void dlist_update(InfoBlockPage* page)
{
    Ihandle* ih = page->disabledlist;
    int val     = IupGetInt(ih, "VALUE");

    if (val > 0)
        IupSetAttribute(page->addbtn, "ACTIVE", "YES");
    else
        IupSetAttribute(page->addbtn, "ACTIVE", "NO");
}

static int dlist_action(Ihandle* ih, char* text, int item, int state)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    dlist_update(page);

    return IUP_DEFAULT;
}

static int upbtn_action(Ihandle* ih)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    int val = IupGetInt(page->enabledlist, "VALUE");

    if (val > 1 && val <= saSize(page->order)) {
        string temp = 0;
        saExtract(&page->order, val - 1, string, &temp);
        saInsert(&page->order, val - 2, string, temp);
        strDestroy(&temp);

        infoblockpageSave(page);
        infoblockpageUpdate(page);
        IupSetInt(page->enabledlist, "VALUE", val - 1);
        elist_update(page);
    }

    return IUP_DEFAULT;
}

static int downbtn_action(Ihandle* ih)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    int val = IupGetInt(page->enabledlist, "VALUE");

    if (val > 0 && val <= saSize(page->order) - 1) {
        string temp = 0;
        saExtract(&page->order, val - 1, string, &temp);
        saInsert(&page->order, val, string, temp);
        strDestroy(&temp);

        infoblockpageSave(page);
        infoblockpageUpdate(page);
        IupSetInt(page->enabledlist, "VALUE", val + 1);
        elist_update(page);
    }

    return IUP_DEFAULT;
}

static int removebtn_action(Ihandle* ih)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    int val = IupGetInt(page->enabledlist, "VALUE");

    if (val > 0 && val <= saSize(page->order)) {
        string temp = 0;
        saExtract(&page->order, val - 1, string, &temp);
        saPushC(&page->orderdisabled, string, &temp);

        infoblockpageSave(page);
        infoblockpageUpdate(page);
        elist_update(page);
        dlist_update(page);
    }

    return IUP_DEFAULT;
}

static int addbtn_action(Ihandle* ih)
{
    InfoBlockPage* page = iupGetParentObj(InfoBlockPage, ih);
    if (!page)
        return IUP_IGNORE;

    int val = IupGetInt(page->disabledlist, "VALUE");

    if (val > 0 && val <= saSize(page->orderdisabled)) {
        string temp = 0;
        saExtract(&page->orderdisabled, val - 1, string, &temp);
        saPushC(&page->order, string, &temp);

        infoblockpageSave(page);
        infoblockpageUpdate(page);
        elist_update(page);
        dlist_update(page);
    }

    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool InfoBlockPage_make(_In_ InfoBlockPage* self, Ihandle* list)
{
    Ihandle* instrlabel = IupLabel(langGetC(self->ss, _S"infoblock_instructions"));

    Ihandle* elabel   = IupLabel(langGetC(self->ss, _S"infoblock_enabled"));
    self->enabledlist = IupList(NULL);
    IupSetAttribute(self->enabledlist, "SIZE", "1x100");
    IupSetAttribute(self->enabledlist, "EXPAND", "HORIZONTAL");
    iupSetObj(self->enabledlist, ObjNone, self, self->ui);
    IupSetCallback(self->enabledlist, "ACTION", (Icallback)elist_action);

    self->upbtn = IupButton("^", NULL);
    IupSetAttribute(self->upbtn, "SIZE", "20x15");
    IupSetAttribute(self->upbtn, "ACTIVE", "NO");
    iupSetObj(self->upbtn, ObjNone, self, self->ui);
    IupSetCallback(self->upbtn, "ACTION", (Icallback)upbtn_action);

    self->downbtn = IupButton("v", NULL);
    IupSetAttribute(self->downbtn, "SIZE", "20x15");
    IupSetAttribute(self->downbtn, "ACTIVE", "NO");
    iupSetObj(self->downbtn, ObjNone, self, self->ui);
    IupSetCallback(self->downbtn, "ACTION", (Icallback)downbtn_action);

    self->removebtn = IupButton("-", NULL);
    IupSetAttribute(self->removebtn, "SIZE", "20x15");
    IupSetAttribute(self->removebtn, "ACTIVE", "NO");
    iupSetObj(self->removebtn, ObjNone, self, self->ui);
    IupSetCallback(self->removebtn, "ACTION", (Icallback)removebtn_action);

    Ihandle* espc = IupSpace();
    IupSetAttribute(espc, "SIZE", "1x8");

    Ihandle* ebtnbox = IupVbox(self->upbtn, self->downbtn, IupFill(), self->removebtn, NULL);
    IupSetAttribute(ebtnbox, "EXPAND", "YES");
    Ihandle* ehbox = IupHbox(self->enabledlist, ebtnbox, NULL);
    IupSetAttribute(ehbox, "CMARGIN", "0x0");
    IupSetAttribute(ehbox, "ALIGNMENT", "ATOP");
    IupSetAttribute(ehbox, "EXPAND", "HORIZONTAL");
    IupSetAttribute(ehbox, "SIZE", "1x100");

    Ihandle* dlabel    = IupLabel(langGetC(self->ss, _S"infoblock_disabled"));
    self->disabledlist = IupList(NULL);
    IupSetAttribute(self->disabledlist, "SIZE", "1x100");
    IupSetAttribute(self->disabledlist, "EXPAND", "HORIZONTAL");
    iupSetObj(self->disabledlist, ObjNone, self, self->ui);
    IupSetCallback(self->disabledlist, "ACTION", (Icallback)dlist_action);

    self->addbtn = IupButton("+", NULL);
    IupSetAttribute(self->addbtn, "SIZE", "20x15");
    IupSetAttribute(self->addbtn, "ACTIVE", "NO");
    iupSetObj(self->addbtn, ObjNone, self, self->ui);
    IupSetCallback(self->addbtn, "ACTION", (Icallback)addbtn_action);

    Ihandle* dspc = IupSpace();
    IupSetAttribute(dspc, "SIZE", "1x8");

    Ihandle* dhbox = IupHbox(self->disabledlist, self->addbtn, NULL);
    IupSetAttribute(dhbox, "CMARGIN", "0x0");
    IupSetAttribute(dhbox, "ALIGNMENT", "ATOP");

    self->h = IupVbox(instrlabel, espc, elabel, ehbox, dspc, dlabel, dhbox, IupFill(), NULL);

    iupLoadImage(self->ss, _S"IMAGE_INFOBLOCK", _S"svg", _S"subspace:/infoblock.svg", list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool InfoBlockPage_update(_In_ InfoBlockPage* self)
{
    string temp = 0;

    // first time; update the order from saved settings
    // This is O(n^2) so only do it once
    if (saSize(self->order) == 0 && saSize(self->orderdisabled) == 0) {
        ssdLockedTransaction(self->feature->settings)
        {
            for (int j = 0; j < numinfoitems; j++) {
                for (int i = 0; i < numinfoitems; i++) {
                    if (ssdVal(int32, self->feature->settings, infoitems[i], 0) == j) {
                        saPush(&self->order, strref, infoitems[i]);
                    }
                }
            }

            for (int i = 0; i < numinfoitems; i++) {
                if (ssdVal(int32, self->feature->settings, infoitems[i], 0) == -1) {
                    saPush(&self->orderdisabled, strref, infoitems[i]);
                }
            }
        }
    }

    IupSetAttribute(self->enabledlist, "1", NULL);
    foreach (sarray, idx, string, iname, self->order) {
        strConcat(&temp, _S"infoblock_", iname);
        IupSetAttribute(self->enabledlist, "APPENDITEM", langGetC(self->ss, temp));
    }

    IupSetAttribute(self->disabledlist, "1", NULL);
    foreach (sarray, idx, string, iname, self->orderdisabled) {
        strConcat(&temp, _S"infoblock_", iname);
        IupSetAttribute(self->disabledlist, "APPENDITEM", langGetC(self->ss, temp));
    }

    strDestroy(&temp);
    return true;
}

void InfoBlockPage_save(_In_ InfoBlockPage* self)
{
    if (saSize(self->order) == 0 && saSize(self->orderdisabled) == 0)
        return;

    ssdLockedTransaction(self->feature->settings)
    {
        foreach (sarray, idx, string, iname, self->order) {
            ssdSet(self->feature->settings, iname, true, stvar(int32, idx));
        }

        IupSetAttribute(self->disabledlist, "1", NULL);
        foreach (sarray, idx, string, iname, self->orderdisabled) {
            ssdSet(self->feature->settings, iname, true, stvar(int32, -1));
        }
    }

    foreach (sarray, idx, string, iname, self->order) {
        featureSendSettingCur(self->feature, iname);
    }
    foreach (sarray, idx, string, iname, self->orderdisabled) {
        featureSendSettingCur(self->feature, iname);
    }
}

// Autogen begins -----
#include "infoblockpage.auto.inc"
// Autogen ends -------
