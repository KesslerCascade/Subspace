// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "runtrackerpage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "runtracker.h"

_objfactory_guaranteed RunTrackerPage* RunTrackerPage_create(RunTracker* feature, SubspaceUI* ui)
{
    RunTrackerPage* self;
    self = objInstCreate(RunTrackerPage);

    self->feature = feature;
    self->owner   = objGetWeak(SubspaceFeature, feature);
    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"runtracker";
    strDup(&self->title, langGet(self->ss, _S"feature_runtracker"));
    strDup(&self->imgname, _S"IMAGE_RUNTRACKER");

    objInstInit(self);
    return self;
}

static int rundirtextchange(Ihandle* ih, int c, char* new_value)
{
    RunTrackerPage* rp = iupGetParentObj(RunTrackerPage, ih);
    if (!rp)
        return IUP_IGNORE;

    string rundirfmt = 0;
    pathFromPlatform(&rundirfmt, (strref)new_value);
    pathNormalize(&rundirfmt);

    if (strEmpty(rundirfmt))
        strDup(&rundirfmt, RUNDIRFMT_DEFAULT);

    ssdSet(rp->feature->settings, _S"rundirfmt", true, stvar(string, rundirfmt));
    strDestroy(&rundirfmt);

    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool RunTrackerPage_make(_In_ RunTrackerPage* self, Ihandle* list)
{
    Ihandle* rundirlabel = IupLabel(langGetC(self->ss, "settings_rundirfmt"));
    IupSetAttribute(rundirlabel, "FONT", "Helvetica, Bold 10");
    Ihandle* rundirlonglabel = IupLabel(langGetC(self->ss, "settings_rundirfmt_long"));
    IupSetAttribute(rundirlonglabel, "SIZE", "1x42");
    IupSetAttribute(rundirlonglabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(rundirlonglabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(rundirlonglabel, "WORDWRAP", "YES");

    self->rundirtext = IupText(NULL);
    IupSetAttribute(self->rundirtext, "EXPAND", "HORIZONTAL");
    iupSetObj(self->rundirtext, ObjNone, self, self->ui);
    IupSetCallback(self->rundirtext, "ACTION", (Icallback)rundirtextchange);

    Ihandle* rundirvarlistlabel = IupLabel(langGetC(self->ss, "settings_rundirvar_list"));
    IupSetAttribute(rundirvarlistlabel, "SIZE", "1x24");
    IupSetAttribute(rundirvarlistlabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(rundirvarlistlabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(rundirvarlistlabel, "WORDWRAP", "YES");

    Ihandle* vbox = IupVbox(rundirlabel,
                            rundirlonglabel,
                            self->rundirtext,
                            rundirvarlistlabel,
                            IupFill(),
                            NULL);
    IupSetAttribute(vbox, "CMARGIN", "6x6");
    IupSetAttribute(vbox, "CGAP", "4");

    self->h = IupBackgroundBox(vbox);

    iupLoadImage(self->ss, _S"IMAGE_RUNTRACKER", _S"svg", _S"subspace:/runtracker.svg", list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool RunTrackerPage_update(_In_ RunTrackerPage* self)
{
    string temp = 0;
    ssdStringOutD(self->feature->settings, _S"rundirfmt", &temp, RUNDIRFMT_DEFAULT);
    pathToPlatform(&temp, temp);
    IupSetStrAttribute(self->rundirtext, "VALUE", strC(temp));

    strDestroy(&temp);
    return true;
}

// Autogen begins -----
#include "runtrackerpage.auto.inc"
// Autogen ends -------
