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
#include <iupcontrols.h>
#include "kbmgr/kbmgr.h"
#include "kbmgr/keymap.h"

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
    return true;
    // Autogen ends -------
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

    Ihandle* kbnote = IupLabel(langGetC(self->ss, _S"keybind_note"));
    IupSetAttribute(kbnote, "SIZE", "1x24");
    IupSetAttribute(kbnote, "WORDWRAP", "YES");
    IupSetAttribute(kbnote, "EXPAND", "HORIZONTAL");
    IupSetAttribute(kbnote, "ALIGNMENT", "ALEFT:ATOP");

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
    IupSetAttribute(self->kbgrid, "SIZE", "260x1");
    IupSetAttribute(self->kbgrid, "BORDER", "YES");
    IupSetAttribute(self->kbgrid, "BGCOLOR", "255 255 255");
    IupSetAttribute(self->kbgrid, "FRAMECOLOR", "255 255 255");
    Ihandle* kbbg = IupBackgroundBox(self->kbgrid);
    IupSetAttribute(kbbg, "BGCOLOR", "255 255 255");

    self->tip = IupLabel("");
    IupSetAttribute(self->tip, "SIZE", "260x16");
    IupSetAttribute(self->tip, "WORDWRAP", "YES");
    IupSetAttribute(self->tip, "ALIGNMENT", "ALEFT:ATOP");

    Ihandle* gb2     = IupVbox(kbbg, self->tip, NULL);
    Ihandle* gridbox = IupHbox(IupFill(), gb2, IupFill(), NULL);

    self->h = IupBackgroundBox(IupVbox(kbinst, kbnote, gridbox, self->tip, IupFill(), NULL));

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

    int row = 1;
    withReadLock (&kbmgr->kbmgrlock) {
        foreach (hashtable, hti, kbmgr->binds) {
            KeyBind* bind = (KeyBind*)htiVal(object, hti);

            SubspaceFeature* feat = objAcquireFromWeak(SubspaceFeature, bind->owner);

            if (!feat || featureIsEnabled(feat)) {
                IupSetAttributeId2(self->kbgrid, "", row, 1, langGetC(self->ss, bind->name));

                string keyname = 0;
                if (htFind(keymap_iuptodisplay, int32, bind->iupkey, string, &keyname)) {
                    IupSetAttributeId2(self->kbgrid, "", row, 2, strC(keyname));
                    strDestroy(&keyname);
                }
                row++;
            }

            objRelease(&feat);
        }
    }
    IupSetInt(self->kbgrid, "NUMLIN", row - 1);
    IupSetAttribute(self->kbgrid, "FITTOSIZE", "COLUMNS");
    IupSetAttribute(self->kbgrid, "REDRAW", "ALL");
}

// Autogen begins -----
#include "keybindspage.auto.inc"
// Autogen ends -------
