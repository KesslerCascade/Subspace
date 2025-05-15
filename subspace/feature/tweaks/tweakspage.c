// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tweakspage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "tweaks.h"

_objfactory_guaranteed TweaksPage* TweaksPage_create(Tweaks* feature, SubspaceUI* ui)
{
    TweaksPage* self;
    self = objInstCreate(TweaksPage);

    self->feature = feature;
    self->owner   = objGetWeak(SubspaceFeature, feature);
    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"tweaks";
    strDup(&self->title, langGet(self->ss, _S"feature_tweaks"));
    strDup(&self->imgname, _S"IMAGE_TWEAKS");

    objInstInit(self);
    return self;
}

static int fpstoggle_action(Ihandle* ih, int state)
{
    TweaksPage* self = iupGetParentObj(TweaksPage, ih);
    if (state != 1 || !self)
        return IUP_DEFAULT;

    if (ih == self->fps30) {
        ssdSet(self->feature->settings, _S"targetfps", true, stvar(int32, 30));
        IupSetAttribute(self->fpscustomval, "VALUE", "");
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else if (ih == self->fps60) {
        ssdSet(self->feature->settings, _S"targetfps", true, stvar(int32, 60));
        IupSetAttribute(self->fpscustomval, "VALUE", "");
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else if (ih == self->fps120) {
        ssdSet(self->feature->settings, _S"targetfps", true, stvar(int32, 120));
        IupSetAttribute(self->fpscustomval, "VALUE", "");
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else {
        IupSetAttribute(self->fpscustomval, "ACTIVE", "YES");
        int val = ssdVal(int32, self->feature->settings, _S"targetfps", 60);
        strFromInt32(&self->fpscustomtext, val, 10);
        IupSetAttribute(self->fpscustomval, "VALUE", strC(self->fpscustomtext));
    }

    featureSendSettingCur(self->feature, _S"targetfps");

    return IUP_DEFAULT;
}

static int fpscustom_action(Ihandle *ih, int c, char *newval)
{
    TweaksPage* self = iupGetParentObj(TweaksPage, ih);
    if (!self)
        return IUP_IGNORE;

    int32 nval = 0;
    if (strToInt32(&nval, (strref)newval, 10, true)) {
        ssdSet(self->feature->settings, _S"targetfps", true, stvar(int32, clamplow(nval, 30)));
        featureSendSettingCur(self->feature, _S"targetfps");
    }

    return IUP_DEFAULT;
}

static int fpscustom_killfocus(Ihandle *ih)
{
    TweaksPage* self = iupGetParentObj(TweaksPage, ih);
    if (!self)
        return IUP_DEFAULT;

    int val = ssdVal(int32, self->feature->settings, _S"targetfps", 60);
    strFromInt32(&self->fpscustomtext, val, 10);
    IupSetAttribute(self->fpscustomval, "VALUE", strC(self->fpscustomtext));

    return IUP_DEFAULT;
}


extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool TweaksPage_make(_In_ TweaksPage* self, Ihandle* list)
{
    Ihandle* fpslabel = IupLabel(langGetC(self->ss, _S"tweaks_targetfps"));
    IupSetAttribute(fpslabel, "FONT", "Helvetica, Bold 10");

    Ihandle* fpsdesc = IupLabel(langGetC(self->ss, _S"tweaks_targetfps_desc"));
    IupSetAttribute(fpsdesc, "SIZE", "1x26");
    IupSetAttribute(fpsdesc, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(fpsdesc, "EXPAND", "HORIZONTAL");
    IupSetAttribute(fpsdesc, "WORDWRAP", "YES");

    self->fps30 = IupToggle(langGetC(self->ss, _S"tweaks_30fps"), NULL);
    iupSetObj(self->fps30, ObjNone, self, self->ui);
    IupSetCallback(self->fps30, "ACTION", (Icallback)fpstoggle_action);
    self->fps60 = IupToggle(langGetC(self->ss, _S"tweaks_60fps"), NULL);
    iupSetObj(self->fps60, ObjNone, self, self->ui);
    IupSetCallback(self->fps60, "ACTION", (Icallback)fpstoggle_action);
    self->fps120 = IupToggle(langGetC(self->ss, _S"tweaks_120fps"), NULL);
    iupSetObj(self->fps120, ObjNone, self, self->ui);
    IupSetCallback(self->fps120, "ACTION", (Icallback)fpstoggle_action);

    self->fpscustom = IupToggle(langGetC(self->ss, _S"tweaks_custom"), NULL);
    iupSetObj(self->fpscustom, ObjNone, self, self->ui);
    IupSetCallback(self->fpscustom, "ACTION", (Icallback)fpstoggle_action);
    self->fpscustomval = IupText(NULL);
    IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    IupSetAttribute(self->fpscustomval, "NC", "4");
    IupSetAttribute(self->fpscustomval, "MASK", IUP_MASK_UINT);
    IupSetAttribute(self->fpscustomval, "VISIBLECOLUMNS", "6");
    iupSetObj(self->fpscustomval, ObjNone, self, self->ui);
    IupSetCallback(self->fpscustomval, "ACTION", (Icallback)fpscustom_action);
    IupSetCallback(self->fpscustomval, "KILLFOCUS_CB", (Icallback)fpscustom_killfocus);

    Ihandle* fpschbox = IupHbox(self->fpscustom, self->fpscustomval, NULL);
    IupSetAttribute(fpschbox, "CGAP", "6");

    Ihandle* fpsvbox = IupVbox(self->fps30, self->fps60, self->fps120, fpschbox, NULL);
    IupSetAttribute(fpsvbox, "CMARGIN", "0x0");
    IupSetAttribute(fpsvbox, "CGAP", "2");

    self->fpsradio = IupRadio(fpsvbox);

    Ihandle* vbox = IupVbox(fpslabel, fpsdesc, self->fpsradio, IupFill(), NULL);
    IupSetAttribute(vbox, "CMARGIN", "6x6");

    self->h = IupBackgroundBox(vbox);

    iupLoadImage(self->ss, _S"IMAGE_TWEAKS", _S"svg", _S"subspace:/tweaks.svg", list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool TweaksPage_update(_In_ TweaksPage* self)
{
    int val = ssdVal(int32, self->feature->settings, _S"targetfps", 60);
    IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    if (val == 30) {
        IupSetAttributeHandle(self->fpsradio, "VALUE", self->fps30);
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else if (val == 60) {
        IupSetAttributeHandle(self->fpsradio, "VALUE", self->fps60);
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else if (val == 120) {
        IupSetAttributeHandle(self->fpsradio, "VALUE", self->fps120);
        IupSetAttribute(self->fpscustomval, "ACTIVE", "NO");
    } else {
        IupSetAttributeHandle(self->fpsradio, "VALUE", self->fpscustom);
        IupSetAttribute(self->fpscustomval, "ACTIVE", "YES");
        strFromInt32(&self->fpscustomtext, val, 10);
        IupSetAttribute(self->fpscustomval, "VALUE", strC(self->fpscustomtext));
    }
    return true;
}

void TweaksPage_destroy(_In_ TweaksPage* self)
{
    // Autogen begins -----
    strDestroy(&self->fpscustomtext);
    // Autogen ends -------
}

// Autogen begins -----
#include "tweakspage.auto.inc"
// Autogen ends -------
