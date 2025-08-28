// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/screenshot/screenshotpage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <iup.h>

_objfactory_guaranteed ScreenshotPage* ScreenshotPage_create(Screenshot* feature, SubspaceUI* ui)
{
    ScreenshotPage* self;
    self = objInstCreate(ScreenshotPage);

    self->feature = feature;
    self->owner   = objGetWeak(SubspaceFeature, feature);
    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"screenshot";
    strDup(&self->title, langGet(self->ss, _S"feature_screenshot"));
    strDup(&self->imgname, _S"IMAGE_SCREENSHOT");

    objInstInit(self);

    self->mloc = ssloccontrolsCreate(self, false);
    self->aloc = ssloccontrolsCreate(self, true);

    return self;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool ScreenshotPage_make(_In_ ScreenshotPage* self, Ihandle* list)
{
    if (!(ssloccontrolsMake(self->mloc) &&ssloccontrolsMake(self->aloc)))
        return false;

    Ihandle* ffmtlabel = IupLabel(langGetC(self->ss, "screenshot_filetype"));
    // IupSetAttribute(fpslabel, "FONT", "Helvetica, Bold 10");
    self->fmtpng       = IupToggle(langGetC(self->ss, "screenshot_png"), "");
    self->fmtjpg       = IupToggle(langGetC(self->ss, "screenshot_jpg"), "");
    Ihandle* qspc      = IupSpace();
    IupSetAttribute(qspc, "SIZE", "12x1");
    Ihandle* qlabel = IupLabel(langGetC(self->ss, "screenshot_jpg_quality"));
    self->jpgqval   = IupVal("HORIZONTAL");
    self->jpgqvalnum   = IupText("");

    Ihandle* fmthbox = IupHbox(ffmtlabel, self->fmtpng, self->fmtjpg, qspc, qlabel, self->jpgqval, self->jpgqvalnum, NULL);
    IupSetAttribute(fmthbox, "ALIGN", "ACENTER");
    IupSetAttribute(fmthbox, "CMARGIN", "0x0");
    IupSetAttribute(fmthbox, "CGAP", "3");

    self->fmtradio = IupRadio(fmthbox);

    Ihandle* fnamelabel = IupLabel(langGetC(self->ss, "screenshot_filenamefmt"));
    IupSetAttribute(fnamelabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(fnamelabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(fnamelabel, "WORDWRAP", "YES");

    self->namefmttext = IupText(NULL);
    IupSetAttribute(self->namefmttext, "EXPAND", "HORIZONTAL");
    iupSetObj(self->namefmttext, ObjNone, self, self->ui);
    // IupSetCallback(self->namefmttext, "ACTION", (Icallback)fnametextchange);
    self->nameextlabel = IupLabel(".ext");
    Ihandle* fnamehbox = IupHbox(self->namefmttext, self->nameextlabel, NULL);
    IupSetAttribute(fnamehbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(fnamehbox, "CMARGIN", "0x0");
    IupSetAttribute(fnamehbox, "CGAP", "3");

    Ihandle* fnamevarlistlabel = IupLabel(langGetC(self->ss, "screenshot_filenamevar_list"));
    IupSetAttribute(fnamevarlistlabel, "SIZE", "1x24");
    IupSetAttribute(fnamevarlistlabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(fnamevarlistlabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(fnamevarlistlabel, "WORDWRAP", "YES");

    self->hidemouse     = IupToggle(langGetC(self->ss, "screenshot_hidemouse"), "");
    self->hidepaused    = IupToggle(langGetC(self->ss, "screenshot_hidepause"), "");
    self->hideinfoblock = IupToggle(langGetC(self->ss, "screenshot_hideinfoblock"), "");
    Ihandle* hideblock  = IupVbox(self->hidemouse, self->hidepaused, self->hideinfoblock, NULL);
    IupSetAttribute(hideblock, "CMARGIN", "0x0");
    IupSetAttribute(hideblock, "CGAP", "1");

    Ihandle* manspc = IupSpace();
    IupSetAttribute(manspc, "SIZE", "1x3");
    Ihandle* mansep = IupLabel("");
    IupSetAttribute(mansep, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(mansep, "EXPAND", "HORIZONTAL");

    Ihandle* manlbl = IupLabel(langGetC(self->ss, "screenshot_manual"));
    IupSetAttribute(manlbl, "FONT", "Helvetica, Bold 10");

    Ihandle* autospc = IupSpace();
    IupSetAttribute(autospc, "SIZE", "1x3");
    Ihandle* autosep = IupLabel("");
    IupSetAttribute(autosep, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(autosep, "EXPAND", "HORIZONTAL");

    Ihandle* autolbl = IupLabel(langGetC(self->ss, "screenshot_auto"));
    IupSetAttribute(autolbl, "FONT", "Helvetica, Bold 10");

    Ihandle* thevbox = IupVbox(self->fmtradio,
                               fnamelabel,
                               fnamehbox,
                               fnamevarlistlabel,
                               hideblock,
                               manspc,
                               mansep,
                               manlbl,
                               self->mloc->h,
                               autospc,
                               autosep,
                               autolbl,
                               self->aloc->h,
                               IupFill(),
                               NULL);
    IupSetAttribute(thevbox, "CMARGIN", "6x6");
    IupSetAttribute(thevbox, "CGAP", "6");

    self->h = IupBackgroundBox(thevbox);

    iupLoadImage(self->ss, _S"IMAGE_SCREENSHOT", _S"svg", _S"subspace:/picture.svg", list);
    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool ScreenshotPage_update(_In_ ScreenshotPage* self)
{
    return true;
}

void ScreenshotPage_updateLoc(_In_ ScreenshotPage* self, SSLocControls* c)
{
    return;
}

void ScreenshotPage_destroy(_In_ ScreenshotPage* self)
{
    // Autogen begins -----
    objRelease(&self->mloc);
    objRelease(&self->aloc);
    // Autogen ends -------
}

_objfactory_guaranteed SSLocControls* SSLocControls_create(ScreenshotPage* page, bool isauto)
{
    SSLocControls* self;
    self = objInstCreate(SSLocControls);

    self->page = page;
    self->ss   = page->ss;
    self->isauto = isauto;

    objInstInit(self);
    return self;
}

bool SSLocControls_make(_In_ SSLocControls* self)
{
    self->global  = IupToggle(langGetC(self->ss, "screenshot_global"), NULL);
    self->globalopen = IupButton(langGetC(self->ss, "screenshot_global_open"), NULL);
    IupSetAttribute(self->globalopen, "CPADDING", "6x1");
    self->rundir  = IupToggle(langGetC(self->ss, "screenshot_rundir"), NULL);
    self->custom  = IupToggle(langGetC(self->ss, "screenshot_custom"), NULL);
    self->ctext      = IupText(NULL);
    IupSetAttribute(self->ctext, "EXPAND", "HORIZONTAL");
    self->cbrowse = IupButton(langGetC(self->ss, "settings_ftl_browse"), NULL);
    IupSetAttribute(self->cbrowse, "CPADDING", "6x1");
    Ihandle* mgbl         = IupHbox(self->global, self->globalopen, NULL);
    IupSetAttribute(mgbl, "CMARGIN", "0x0");
    IupSetAttribute(mgbl, "CGAP", "6");
    IupSetAttribute(mgbl, "ALIGNMENT", "ACENTER");
    Ihandle* mcust         = IupHbox(self->custom, self->ctext, self->cbrowse, NULL);
    IupSetAttribute(mcust, "CMARGIN", "0x0");
    IupSetAttribute(mcust, "CGAP", "6");
    IupSetAttribute(mcust, "ALIGNMENT", "ACENTER");
    Ihandle* mlocstack = IupVbox(mgbl, self->rundir, mcust, NULL);
    IupSetAttribute(mlocstack, "CMARGIN", "0x0");
    IupSetAttribute(mlocstack, "CGAP", "1");
    
    self->h = IupRadio(mlocstack);

    return true;
}

bool SSLocControls_update(_In_ SSLocControls* self)
{
    return false;
}

// Autogen begins -----
#include "feature/screenshot/screenshotpage.auto.inc"
// Autogen ends -------
