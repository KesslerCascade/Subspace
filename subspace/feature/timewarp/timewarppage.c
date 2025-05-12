// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "timewarppage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <math.h>
#include "feature/featureregistry.h"
#include "timewarp.h"

static int npow2(int val)
{
    if (val <= 0)
        return 1;

    int ret = 2;
    for (int i = 1; i < val; i++) {
        ret <<= 1;
    }
    return ret;
}

static int nlog2(int val) {
    if (val <= 1)
        return 0;
    int ret = 1;

    while (val > 2) {
        val >>= 1;
        ret++;
    }

    return ret;
}

_objfactory_guaranteed TimeWarpPage* TimeWarpPage_create(TimeWarp* feature, SubspaceUI* ui)
{
    TimeWarpPage* self;
    self = objInstCreate(TimeWarpPage);

    self->feature = feature;
    self->ui      = ui;
    self->ss      = ui->ss;
    self->visible = true;

    self->name = _S"timewarp";
    strDup(&self->title, langGet(self->ss, _S"feature_timewarp"));
    strDup(&self->imgname, _S"IMAGE_TIMEWARP");

    objInstInit(self);
    return self;
}

static void setMaxWarpLabel(TimeWarpPage* self, int val)
{
    strFormat(&self->maxwarptext, _S"${int}x", stvar(int32, npow2(val)));
    IupSetAttribute(self->maxwarp_val_num, "TITLE", strC(self->maxwarptext));
}

static void setDoubleTapLabel(TimeWarpPage* self, int val)
{
    if (val == 0)
        strDup(&self->doubletaptext, langGet(self->ss, _S"timewarp_doubletap_off"));
    else
        strFormat(&self->doubletaptext, _S"${int}x", stvar(int32, npow2(val)));
    IupSetAttribute(self->doubletap_val_num, "TITLE", strC(self->doubletaptext));
}

static int val_changed(Ihandle* ih)
{
    TimeWarpPage* page   = iupGetParentObj(TimeWarpPage, ih);
    const char* whichval = IupGetAttribute(ih, "TIMEWARP_VAL");

    if (!page || !whichval)
        return IUP_IGNORE;

    TimeWarp* feat = page->feature;
    char* valstr   = IupGetAttribute(ih, "VALUE");
    int minval     = IupGetInt(ih, "MIN");
    int maxval     = IupGetInt(ih, "MAX");

    float64 val = atof(valstr);
    int nval    = clamp(round(val), minval, maxval);
    IupSetInt(ih, "VALUE", nval);

    if (!strcmp(whichval, "MAXWARP")) {
        ssdSet(feat->settings, _S"maxwarp", false, stvar(int32, npow2(nval)));
        setMaxWarpLabel(page, nval);
        featureSendSettingCur(feat, _S"maxwarp");
    } else if (!strcmp(whichval, "DOUBLETAP")) {
        ssdSet(feat->settings, _S"doubletap", false, stvar(int32, nval == 0 ? 0 : npow2(nval)));
        setDoubleTapLabel(page, nval);
        featureSendSettingCur(feat, _S"doubletap");
    }

    return IUP_DEFAULT;
}

static int slowmo_toggle(Ihandle* ih, int state)
{
    TimeWarpPage* page = iupGetParentObj(TimeWarpPage, ih);
    if (!page)
        return IUP_IGNORE;
    TimeWarp* feat = page->feature;

    ssdSet(feat->settings, _S"allowslowmo", false, stvar(bool, state ? true : false));
    featureSendSettingCur(feat, _S"allowslowmo");
    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool TimeWarpPage_make(_In_ TimeWarpPage* self, Ihandle* list)
{
    Ihandle* maxwarplbl = IupLabel(langGetC(self->ss, _S"timewarp_maxwarp"));
    self->maxwarp_val   = IupVal("HORIZONTAL");
    IupSetAttribute(self->maxwarp_val, "MIN", "1");
    IupSetAttribute(self->maxwarp_val, "MAX", "5");
    IupSetAttribute(self->maxwarp_val, "SHOWTICKS", "5");
    IupSetAttribute(self->maxwarp_val, "STEP", "0.25");
    IupSetAttribute(self->maxwarp_val, "PAGESTEP", "0.25");
    IupSetAttribute(self->maxwarp_val, "MINSIZE", "200x0");
    IupSetCallback(self->maxwarp_val, "VALUECHANGED_CB", (Icallback)val_changed);
    IupSetAttribute(self->maxwarp_val, "TIMEWARP_VAL", "MAXWARP");
    iupSetObj(self->maxwarp_val, ObjNone, self, self->ui);
    setTip(self->maxwarp_val,
           langGet(self->ss, _S"timewarp_maxwarp_tip"),
           langGet(self->ss, _S"timewarp_maxwarp"),
           1);
    self->maxwarp_val_num = IupLabel("32");
    IupSetAttribute(self->maxwarp_val_num, "EXPAND", "HORIZONTAL");

    Ihandle* doubletaplbl = IupLabel(langGetC(self->ss, _S"timewarp_doubletap"));
    self->doubletap_val   = IupVal("HORIZONTAL");
    IupSetAttribute(self->doubletap_val, "MIN", "0");
    IupSetAttribute(self->doubletap_val, "MAX", "5");
    IupSetAttribute(self->doubletap_val, "SHOWTICKS", "6");
    IupSetAttribute(self->doubletap_val, "STEP", "0.2");
    IupSetAttribute(self->doubletap_val, "PAGESTEP", "0.2");
    IupSetAttribute(self->doubletap_val, "MINSIZE", "200x0");
    IupSetCallback(self->doubletap_val, "VALUECHANGED_CB", (Icallback)val_changed);
    IupSetAttribute(self->doubletap_val, "TIMEWARP_VAL", "DOUBLETAP");
    iupSetObj(self->doubletap_val, ObjNone, self, self->ui);
    setTip(self->doubletap_val,
           langGet(self->ss, _S"timewarp_doubletap_tip"),
           langGet(self->ss, _S"timewarp_doubletap"),
           1);
    self->doubletap_val_num = IupLabel(langGetC(self->ss, _S"timewarp_doubletap_off"));
    IupSetAttribute(self->doubletap_val_num, "EXPAND", "HORIZONTAL");

    Ihandle* gbox = IupGridBox(maxwarplbl,
                               self->maxwarp_val,
                               self->maxwarp_val_num,
                               doubletaplbl,
                               self->doubletap_val,
                               self->doubletap_val_num,
                               NULL);
    IupSetAttribute(gbox, "ORIENTATION", "HOROZINTAL");
    IupSetAttribute(gbox, "ALIGNMENTLIN", "ABOTTOM");
    IupSetAttribute(gbox, "NUMDIV", "3");
    IupSetAttribute(gbox, "CMARGIN", "0x0");
    IupSetAttribute(gbox, "CGAPLIN", "6");
    IupSetAttribute(gbox, "CGAPCOL", "4");
    IupSetAttribute(gbox, "SIZECOL", "-1");
    IupSetAttribute(gbox, "SIZELIN", "-1");

    Ihandle* sep1 = IupSpace();
    IupSetAttribute(sep1, "SIZE", "1x6");

    self->allowslowmo = IupToggle(langGetC(self->ss, _S"timewarp_allowslowmo"), NULL);
    iupSetObj(self->allowslowmo, ObjNone, self, self->ui);
    IupSetCallback(self->allowslowmo, "ACTION", (Icallback)slowmo_toggle);
    setTip(self->allowslowmo,
           langGet(self->ss, _S"timewarp_allowslowmo_tip"),
           langGet(self->ss, _S"timewarp_allowslowmo"),
           1);

    Ihandle* thevbox = IupVbox(gbox, sep1, self->allowslowmo, IupFill(), NULL);
    IupSetAttribute(thevbox, "CMARGIN", "4x4");
    IupSetAttribute(thevbox, "CGAP", "6");
    self->h = IupBackgroundBox(thevbox);

    iupLoadImage(self->ss, _S"IMAGE_TIMEWARP", _S"svg", _S"subspace:/hourglass.svg", list);
    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool TimeWarpPage_update(_In_ TimeWarpPage* self)
{
    // load in the current values
    TimeWarp* feat = self->feature;
    withReadLock (&feat->lock) {
        int val = nlog2(ssdVal(int32, feat->settings,_S"maxwarp", 32));
        IupSetInt(self->maxwarp_val, "VALUE", val);
        setMaxWarpLabel(self, val);

        val = nlog2(ssdVal(int32, feat->settings, _S"doubletap", 0));
        IupSetInt(self->doubletap_val, "VALUE", val);
        setDoubleTapLabel(self, val);

        IupSetAttribute(self->allowslowmo,
                        "VALUE",
                        ssdVal(bool, feat->settings, _S"allowslowmo", false) ? "ON" : "OFF");
    }

    return true;
}

void TimeWarpPage_destroy(_In_ TimeWarpPage* self)
{
    // Autogen begins -----
    strDestroy(&self->maxwarptext);
    strDestroy(&self->doubletaptext);
    // Autogen ends -------
}

// Autogen begins -----
#include "timewarppage.auto.inc"
// Autogen ends -------
