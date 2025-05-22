// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "keybindspage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <iupcontrols.h>
#include "kbmgr/kbmgr.h"
#include "kbmgr/keymap.h"

static const char* bindsbg    = "255 255 255";
static const char* bindselect = "173 216 230";
static const char* binding    = "255 184 28";

_objfactory_guaranteed KeybindsPage* KeybindsPage_create(SubspaceUI* ui)
{
    KeybindsPage* self;
    self = objInstCreate(KeybindsPage);

    self->ui = ui;
    self->ss = ui->ss;

    self->name = _S"keybinds";
    strDup(&self->title, langGet(self->ss, _S"settings_keybinds"));
    strDup(&self->imgname, _S"IMAGE_KEYCAP_SMALL");

    objInstInit(self);

    return self;
}

_objinit_guaranteed bool KeybindsPage_init(_In_ KeybindsPage* self)
{
    // Autogen begins -----
    saInit(&self->binds, object, 1);
    return true;
    // Autogen ends -------
}

static int kbgrid_mousemove(Ihandle* ih, int lin, int col)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);

    keybindspageSetHover(self, lin);

    return IUP_DEFAULT;
}

static int kbgrid_leavewindow(Ihandle* ih)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);

    keybindspageClearHover(self);

    return IUP_DEFAULT;
}

static int kbgrid_click(Ihandle* ih, int lin, int col, char* status)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);

    keybindspageClearHover(self);
    keybindspageStartBind(self, lin);

    return IUP_DEFAULT;
}

static int kbgrid_killfocus(Ihandle* ih)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);

    keybindspageClearHover(self);
    keybindspageCancelBind(self);

    return IUP_DEFAULT;
}

static int kbgrid_key(Ihandle* ih, int c)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);

    if (self->binding) {
        if (self->bindrow > 0 && self->bindrow <= saSize(self->binds)) {
            KeyBind* bind = self->binds.a[self->bindrow - 1];

            if (c == K_ESC)
                c = 0;   // unbind

            kbmgrBind(self->ss->kbmgr, bind->name, c);
        }

        keybindspageCancelBind(self);
        return IUP_IGNORE;
    } else if (self->hoverrow != 0 && (c == K_SP || c == K_CR)) {
        int row = self->hoverrow;
        keybindspageClearHover(self);
        keybindspageStartBind(self, row);
    }

    return IUP_DEFAULT;
}

static int everythingelse_button(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
    KeybindsPage* self = iupGetParentObj(KeybindsPage, ih);
    if (self)
        keybindspageCancelBind(self);

    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool KeybindsPage_make(_In_ KeybindsPage* self, Ihandle* list)
{
    Ihandle* kbinst = IupLabel(langGetC(self->ss, _S"keybind_instructions"));
    IupSetAttribute(kbinst, "SIZE", "1x24");
    IupSetAttribute(kbinst, "WORDWRAP", "YES");
    IupSetAttribute(kbinst, "EXPAND", "HORIZONTAL");
    IupSetAttribute(kbinst, "ALIGNMENT", "ALEFT:ATOP");
    iupSetObj(kbinst, ObjNone, self, self->ui);
    IupSetCallback(kbinst, "BUTTON_CB", (Icallback)everythingelse_button);

    Ihandle* kbnote = IupLabel(langGetC(self->ss, _S"keybind_note"));
    IupSetAttribute(kbnote, "SIZE", "1x48");
    IupSetAttribute(kbnote, "WORDWRAP", "YES");
    IupSetAttribute(kbnote, "EXPAND", "HORIZONTAL");
    IupSetAttribute(kbnote, "ALIGNMENT", "ALEFT:ATOP");
    iupSetObj(kbnote, ObjNone, self, self->ui);
    IupSetCallback(kbnote, "BUTTON_CB", (Icallback)everythingelse_button);

    self->kbgrid = IupMatrix(NULL);
    IupSetAttribute(self->kbgrid, "CURSOR", "ARROW");
    IupSetAttribute(self->kbgrid, "FLAT", "YES");
    IupSetAttribute(self->kbgrid, "READONLY", "YES");
    IupSetAttribute(self->kbgrid, "HIDEFOCUS", "YES");
    IupSetAttribute(self->kbgrid, "NUMCOL", "2");
    IupSetAttribute(self->kbgrid, "ALIGNMENT", "ALEFT");
    IupSetAttribute(self->kbgrid, "FONT*:2", "Courier, Bold 11");
    IupSetAttribute(self->kbgrid, "WIDTH2", "40");
    IupSetAttribute(self->kbgrid, "EXPAND", "VERTICAL");
    IupSetAttribute(self->kbgrid, "SCROLLBAR", "VERTICAL");
    IupSetAttribute(self->kbgrid, "YAUTOHIDE", "NO");
    IupSetAttribute(self->kbgrid, "SIZE", "260x1");
    IupSetAttribute(self->kbgrid, "BORDER", "YES");
    IupSetAttribute(self->kbgrid, "BGCOLOR", "255 255 255");
    IupSetAttribute(self->kbgrid, "FRAMECOLOR", "255 255 255");
    iupSetObj(self->kbgrid, ObjNone, self, self->ui);
    IupSetCallback(self->kbgrid, "MOUSEMOVE_CB", (Icallback)kbgrid_mousemove);
    IupSetCallback(self->kbgrid, "LEAVEWINDOW_CB", (Icallback)kbgrid_leavewindow);
    IupSetCallback(self->kbgrid, "ENTERITEM_CB", (Icallback)kbgrid_mousemove);
    IupSetCallback(self->kbgrid, "RELEASE_CB", (Icallback)kbgrid_click);
    IupSetCallback(self->kbgrid, "KILLFOCUS_CB", (Icallback)kbgrid_killfocus);
    IupSetCallback(self->kbgrid, "K_ANY", (Icallback)kbgrid_key);

    Ihandle* kbbg = IupBackgroundBox(self->kbgrid);
    IupSetAttribute(kbbg, "BGCOLOR", "255 255 255");
    iupSetObj(kbbg, ObjNone, self, self->ui);
    IupSetCallback(kbbg, "BUTTON_CB", (Icallback)everythingelse_button);

    self->tip = IupLabel("");
    IupSetAttribute(self->tip, "SIZE", "260x16");
    IupSetAttribute(self->tip, "WORDWRAP", "YES");
    IupSetAttribute(self->tip, "ALIGNMENT", "ALEFT:ATOP");
    iupSetObj(self->tip, ObjNone, self, self->ui);
    IupSetCallback(self->tip, "BUTTON_CB", (Icallback)everythingelse_button);

    Ihandle* gb2     = IupVbox(kbbg, self->tip, NULL);
    Ihandle* gridbox = IupHbox(IupFill(), gb2, IupFill(), NULL);
    IupSetAttribute(gridbox, "CMARGIN", "0x0");

    Ihandle* vbox = IupVbox(kbinst, kbnote, gridbox, self->tip, IupFill(), NULL);
    IupSetAttribute(vbox, "CMARGIN", "6x6");
    self->h = IupBackgroundBox(vbox);
    iupSetObj(self->h, ObjNone, self, self->ui);
    IupSetCallback(self->h, "BUTTON_CB", (Icallback)everythingelse_button);

    keybindspagePopulateGrid(self);

    iupLoadImage(self->ss, _S"IMAGE_KEYCAP_SMALL", _S"svg", _S"subspace:/keycap-small.svg", list);
    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool KeybindsPage_update(_In_ KeybindsPage* self)
{
    keybindspagePopulateGrid(self);
    return true;
}

void KeybindsPage_populateGrid(_In_ KeybindsPage* self)
{
    KBMgr* kbmgr = self->ss->kbmgr;
    keymapInit();

    IupSetAttribute(self->kbgrid, "CLEARVALUE", "ALL");
    saClear(&self->binds);

    withReadLock (&kbmgr->kbmgrlock) {
        foreach (hashtable, hti, kbmgr->binds) {
            KeyBind* bind         = (KeyBind*)htiVal(object, hti);
            SubspaceFeature* feat = objAcquireFromWeak(SubspaceFeature, bind->owner);
            if (!feat || featureIsEnabled(feat)) {
                saPush(&self->binds, object, bind);
            }
            objRelease(&feat);
        }

        IupSetInt(self->kbgrid, "NUMLIN", saSize(self->binds));

        foreach (sarray, idx, KeyBind*, bind, self->binds) {
            IupSetAttributeId2(self->kbgrid, "", idx + 1, 1, langGetC(self->ss, bind->name));

            string keyname = 0;
            if (bind->iupkey == 0) {
                IupSetAttributeId2(self->kbgrid, "", idx + 1, 2, "");
            } else if (htFind(keymap_iuptodisplay, int32, bind->iupkey, string, &keyname)) {
                IupSetAttributeId2(self->kbgrid, "", idx + 1, 2, strC(keyname));
                strDestroy(&keyname);
            }
        }
    }
    IupSetAttribute(self->kbgrid, "FITTOSIZE", "COLUMNS");
    IupSetAttribute(self->kbgrid, "REDRAW", "ALL");
}

void KeybindsPage_setHover(_In_ KeybindsPage* self, int row)
{
    if (self->bindrow || self->hoverrow == row || row < 1 || row > saSize(self->binds))
        return;

    keybindspageClearHover(self);

    self->hoverrow = row;
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", row, 1, bindselect);
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", row, 2, bindselect);
    IupSetAttributeId2(self->kbgrid, "FRAMEVERTCOLOR", row, 1, bindselect);

    string temp = 0;
    strFormat(&temp, _S"L${int}", stvar(int32, row));
    IupSetAttribute(self->kbgrid, "REDRAW", strC(temp));

    strConcat(&temp, self->binds.a[row - 1]->name, _S"_tip");
    IupSetAttribute(self->tip, "TITLE", langGetC(self->ss, temp));
    strDestroy(&temp);
}

void KeybindsPage_clearHover(_In_ KeybindsPage* self)
{
    if (self->hoverrow == 0)
        return;

    IupSetAttributeId2(self->kbgrid, "BGCOLOR", self->hoverrow, 1, NULL);
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", self->hoverrow, 2, NULL);
    IupSetAttributeId2(self->kbgrid, "FRAMEVERTCOLOR", self->hoverrow, 1, NULL);

    string temp = 0;
    strFormat(&temp, _S"L${int}", stvar(int32, self->hoverrow));
    IupSetAttribute(self->kbgrid, "REDRAW", strC(temp));
    strDestroy(&temp);
    self->hoverrow = 0;

    IupSetAttribute(self->tip, "TITLE", "");
}

void KeybindsPage_setBindRow(_In_ KeybindsPage* self, int row)
{
    keybindspageClearHover(self);
    keybindspageClearBindRow(self);

    self->bindrow = row;
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", row, 1, binding);
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", row, 2, binding);
    IupSetAttributeId2(self->kbgrid, "FRAMEVERTCOLOR", row, 1, binding);

    string temp = 0;
    strFormat(&temp, _S"L${int}", stvar(int32, row));
    IupSetAttribute(self->kbgrid, "REDRAW", strC(temp));

    strConcat(&temp, self->binds.a[row - 1]->name, _S"_tip");
    IupSetAttribute(self->tip, "TITLE", langGetC(self->ss, temp));
    strDestroy(&temp);
}

void KeybindsPage_clearBindRow(_In_ KeybindsPage* self)
{
    if (self->bindrow == 0)
        return;

    IupSetAttributeId2(self->kbgrid, "BGCOLOR", self->bindrow, 1, NULL);
    IupSetAttributeId2(self->kbgrid, "BGCOLOR", self->bindrow, 2, NULL);
    IupSetAttributeId2(self->kbgrid, "FRAMEVERTCOLOR", self->bindrow, 1, NULL);

    string temp = 0;
    strFormat(&temp, _S"L${int}", stvar(int32, self->bindrow));
    IupSetAttribute(self->kbgrid, "REDRAW", strC(temp));
    strDestroy(&temp);
    self->bindrow = 0;

    IupSetAttribute(self->tip, "TITLE", "");
}

void KeybindsPage_destroy(_In_ KeybindsPage* self)
{
    // Autogen begins -----
    saDestroy(&self->binds);
    // Autogen ends -------
}

void KeybindsPage_startBind(_In_ KeybindsPage* self, int row)
{
    if (self->binding)
        keybindspageCancelBind(self);

    self->binding = true;
    IupSetAttributeId2(self->kbgrid, "", row, 2, "...");
    keybindspageSetBindRow(self, row);
}

void KeybindsPage_cancelBind(_In_ KeybindsPage* self)
{
    if (!self->binding)
        return;

    self->binding = false;
    if (self->bindrow > 0 && self->bindrow <= sizeof(self->binds)) {
        string keyname = 0;
        KeyBind* bind  = self->binds.a[self->bindrow - 1];
        if (bind->iupkey == 0) {
            IupSetAttributeId2(self->kbgrid, "", self->bindrow, 2, "");
        } else if (htFind(keymap_iuptodisplay, int32, bind->iupkey, string, &keyname)) {
            IupSetAttributeId2(self->kbgrid, "", self->bindrow, 2, strC(keyname));
            strDestroy(&keyname);
        }
    }
    keybindspageClearBindRow(self);
}

// Autogen begins -----
#include "keybindspage.auto.inc"
// Autogen ends -------
