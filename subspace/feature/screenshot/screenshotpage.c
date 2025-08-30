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
#include <cx/format.h>
#include <iup.h>
#include <math.h>
#include "feature/featureregistry.h"
#include "kbmgr/kbmgr.h"
#include "kbmgr/keymap.h"

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

static void sskbname(Subspace* ss, string* out)
{
    int key = kbmgrGet(ss->kbmgr, _S"screenshot_take");
    if (!htFind(keymap_iuptodisplay, int32, key, string, out))
        strDup(out, langGet(ss, _S"screenshot_none"));
}

static int screenshot_format_radio_action(Ihandle* ih, int state)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    strref newfmt = _S"png";
    if (ih == self->fmtjpg)
        newfmt = _S"jpeg";

    ssdSet(self->feature->settings, _S"format", true, stvar(strref, newfmt));
    screenshotpageUpdateFormatActive(self, newfmt);

    return IUP_DEFAULT;
}

static int screenshot_toggle_action(Ihandle* ih, int state)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    strref setting = 0;
    if (ih == self->sound)
        setting = _S"sound";
    else if (ih == self->hidemouse)
        setting = _S"hidemouse";
    else if (ih == self->hidepause)
        setting = _S"hidepause";
    else if (ih == self->hideinfoblock)
        setting = _S"hideinfoblock";
    else if (ih == self->metadata)
        setting = _S"metadata";
    if (!setting)
        return IUP_IGNORE;

    ssdSet(self->feature->settings, setting, true, stvar(bool, state ? true : false));
    featureSendSettingCur(self->feature, setting);
    return IUP_DEFAULT;
}

static int screenshot_event_action(Ihandle* ih, int state)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    int bit = 0;
    if (ih == self->autoach)
        bit = SSEvent_Ach;
    else if (ih == self->autorfs1)
        bit = SSEvent_RFS1;
    else if (ih == self->autorfs2)
        bit = SSEvent_RFS2;
    else if (ih == self->autorfs3)
        bit = SSEvent_RFS3;
    else if (ih == self->autowinfight)
        bit = SSEvent_WinFight;
    else if (ih == self->autodestroyed)
        bit = SSEvent_Destroyed;
    else if (ih == self->autogameover)
        bit = SSEvent_GameOver;
    else if (ih == self->autovictory)
        bit = SSEvent_Victory;
    else if (ih == self->autocredits)
        bit = SSEvent_Credits;
    if (!bit)
        return IUP_IGNORE;

    SSDNode* sets = self->feature->settings;
    ssdLockedTransaction(sets)
    {
        int32 ev = ssdVal(int32, sets, _S"events", 0);
        if (state)
            ev |= bit;
        else
            ev &= ~bit;
        ssdSet(sets, _S"events", true, stvar(int32, ev));
    }

    featureSendSettingCur(self->feature, _S"events");
    return IUP_DEFAULT;
}

static int screenshot_jpgq_val(Ihandle* ih)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    char* valstr = IupGetAttribute(ih, "VALUE");
    int minval   = IupGetInt(ih, "MIN");
    int maxval   = IupGetInt(ih, "MAX");

    float64 val = atof(valstr);
    int nval    = clamp(round(val), minval, maxval);
    IupSetInt(ih, "VALUE", nval);

    ssdSet(self->feature->settings, _S"jpegquality", true, stvar(int32, nval));
    IupSetInt(self->jpgqvalnum, "VALUE", nval);

    return IUP_DEFAULT;
}

static int screenshot_jpgq_valtext(Ihandle* ih, int c, char* new_value)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    int tval = atoi(new_value);
    int nval = clamp(tval, 0, 100);

    ssdSet(self->feature->settings, _S"jpegquality", true, stvar(int32, nval));
    IupSetInt(self->jpgqval, "VALUE", nval);
    if (tval != nval) {
        IupSetInt(ih, "VALUE", nval);
        return IUP_IGNORE;
    }

    return IUP_DEFAULT;
}

static int screenshot_filename_text(Ihandle* ih, int c, char* new_value)
{
    ScreenshotPage* self = iupGetParentObj(ScreenshotPage, ih);
    if (!self)
        return IUP_IGNORE;

    string fname = 0;
    strCopy(&fname, (strref)new_value);
    if (strEmpty(fname))
        strDup(&fname, SCREENSHOTNAME_DEFAULT);

    ssdSet(self->feature->settings, _S"filename", true, stvar(string, fname));
    strDestroy(&fname);

    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool ScreenshotPage_make(_In_ ScreenshotPage* self, Ihandle* list)
{
    if (!(ssloccontrolsMake(self->mloc) && ssloccontrolsMake(self->aloc)))
        return false;

    string temp = 0;

    Ihandle* ffmtlabel = IupLabel(langGetC(self->ss, "screenshot_filetype"));
    // IupSetAttribute(fpslabel, "FONT", "Helvetica, Bold 10");
    self->fmtpng       = IupToggle(langGetC(self->ss, "screenshot_png"), "");
    iupSetObj(self->fmtpng, ObjNone, self, self->ss->ui);
    IupSetCallback(self->fmtpng, "ACTION", (Icallback)screenshot_format_radio_action);
    self->fmtjpg = IupToggle(langGetC(self->ss, "screenshot_jpg"), "");
    iupSetObj(self->fmtjpg, ObjNone, self, self->ss->ui);
    IupSetCallback(self->fmtjpg, "ACTION", (Icallback)screenshot_format_radio_action);

    Ihandle* qspc = IupSpace();
    IupSetAttribute(qspc, "SIZE", "12x1");
    Ihandle* qlabel = IupLabel(langGetC(self->ss, "screenshot_jpg_quality"));
    self->jpgqval   = IupVal("HORIZONTAL");
    IupSetStrAttribute(self->jpgqval, "MIN", "0");
    IupSetStrAttribute(self->jpgqval, "MAX", "100");
    IupSetStrAttribute(self->jpgqval, "SHOWTICKS", "5");
    IupSetStrAttribute(self->jpgqval, "EXPAND", "HORIZONTAL");
    iupSetObj(self->jpgqval, ObjNone, self, self->ui);
    IupSetCallback(self->jpgqval, "VALUECHANGED_CB", (Icallback)screenshot_jpgq_val);
    self->jpgqvalnum = IupText("");
    iupSetObj(self->jpgqvalnum, ObjNone, self, self->ui);
    IupSetCallback(self->jpgqvalnum, "ACTION", (Icallback)screenshot_jpgq_valtext);

    Ihandle* fmthbox = IupHbox(ffmtlabel,
                               self->fmtpng,
                               self->fmtjpg,
                               qspc,
                               qlabel,
                               self->jpgqval,
                               self->jpgqvalnum,
                               NULL);
    IupSetAttribute(fmthbox, "ALIGNMENT", "ACENTER");
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
    IupSetCallback(self->namefmttext, "ACTION", (Icallback)screenshot_filename_text);

    // IupSetCallback(self->namefmttext, "ACTION", (Icallback)fnametextchange);
    self->nameextlabel = IupLabel(".ext");
    Ihandle* fnamehbox = IupHbox(self->namefmttext, self->nameextlabel, NULL);
    IupSetAttribute(fnamehbox, "ALIGNMENT", "ACENTER");
    IupSetAttribute(fnamehbox, "CMARGIN", "0x0");
    IupSetAttribute(fnamehbox, "CGAP", "3");

    Ihandle* fnamevbox = IupVbox(fnamelabel, fnamehbox);
    IupSetAttribute(fnamevbox, "CMARGIN", "0x0");
    IupSetAttribute(fnamevbox, "CGAP", "1");

    Ihandle* fnamevarlistlabel = IupLabel(langGetC(self->ss, "screenshot_filenamevar_list"));
    IupSetAttribute(fnamevarlistlabel, "SIZE", "1x24");
    IupSetAttribute(fnamevarlistlabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(fnamevarlistlabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(fnamevarlistlabel, "WORDWRAP", "YES");

    self->sound = IupToggle(langGetC(self->ss, "screenshot_sound"), "");
    iupSetObj(self->sound, ObjNone, self, self->ui);
    IupSetCallback(self->sound, "ACTION", (Icallback)screenshot_toggle_action);

    self->hidemouse = IupToggle(langGetC(self->ss, "screenshot_hidemouse"), "");
    setTip(self->hidemouse,
           langGet(self->ss, _S"screenshot_hidemouse_tip"),
           langGet(self->ss, _S"screenshot_hidemouse"),
           0);
    iupSetObj(self->hidemouse, ObjNone, self, self->ui);
    IupSetCallback(self->hidemouse, "ACTION", (Icallback)screenshot_toggle_action);

    self->hidepause = IupToggle(langGetC(self->ss, "screenshot_hidepause"), "");
    setTip(self->hidepause,
           langGet(self->ss, _S"screenshot_hidepause_tip"),
           langGet(self->ss, _S"screenshot_hidepause"),
           0);
    iupSetObj(self->hidepause, ObjNone, self, self->ui);
    IupSetCallback(self->hidepause, "ACTION", (Icallback)screenshot_toggle_action);

    self->hideinfoblock = IupToggle(langGetC(self->ss, "screenshot_hideinfoblock"), "");
    setTip(self->hideinfoblock,
           langGet(self->ss, _S"screenshot_hideinfoblock_tip"),
           langGet(self->ss, _S"screenshot_hideinfoblock"),
           0);
    iupSetObj(self->hideinfoblock, ObjNone, self, self->ui);
    IupSetCallback(self->hideinfoblock, "ACTION", (Icallback)screenshot_toggle_action);

    self->metadata = IupToggle(langGetC(self->ss, "screenshot_metadata"), "");
    setTip(self->metadata,
           langGet(self->ss, _S"screenshot_metadata_tip"),
           langGet(self->ss, _S"metadata"),
           0);
    iupSetObj(self->metadata, ObjNone, self, self->ui);
    IupSetCallback(self->metadata, "ACTION", (Icallback)screenshot_toggle_action);

    Ihandle* toggleblock = IupVbox(self->sound,
                                   self->hidemouse,
                                   self->hidepause,
                                   self->hideinfoblock,
                                   self->metadata,
                                   NULL);
    IupSetAttribute(toggleblock, "CMARGIN", "0x0");
    IupSetAttribute(toggleblock, "CGAP", "1");

    Ihandle* manspc = IupSpace();
    IupSetAttribute(manspc, "SIZE", "1x3");
    Ihandle* mansep = IupLabel("");
    IupSetAttribute(mansep, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(mansep, "EXPAND", "HORIZONTAL");

    string kbname = 0;
    sskbname(self->ss, &kbname);
    strFormat(&temp, langGet(self->ss, _S"screenshot_manual"), stvar(string, kbname));
    strDestroy(&kbname);
    self->manlbl = IupLabel(strC(temp));
    IupSetAttribute(self->manlbl, "FONT", "Helvetica, Bold 10");
    IupSetAttribute(self->manlbl, "EXPAND", "HORIZONTAL");

    Ihandle* autospc = IupSpace();
    IupSetAttribute(autospc, "SIZE", "1x3");
    Ihandle* autosep = IupLabel("");
    IupSetAttribute(autosep, "SEPARATOR", "HORIZONTAL");
    IupSetAttribute(autosep, "EXPAND", "HORIZONTAL");

    Ihandle* autolbl = IupLabel(langGetC(self->ss, "screenshot_auto"));
    IupSetAttribute(autolbl, "FONT", "Helvetica, Bold 10");

    self->autoach = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_ach")), NULL);
    iupSetObj(self->autoach, ObjNone, self, self->ui);
    IupSetCallback(self->autoach, "ACTION", (Icallback)screenshot_event_action);
    self->autorfs1 = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_rfs1")), NULL);
    iupSetObj(self->autorfs1, ObjNone, self, self->ui);
    IupSetCallback(self->autorfs1, "ACTION", (Icallback)screenshot_event_action);
    self->autorfs2 = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_rfs2")), NULL);
    iupSetObj(self->autorfs2, ObjNone, self, self->ui);
    IupSetCallback(self->autorfs2, "ACTION", (Icallback)screenshot_event_action);
    self->autorfs3 = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_rfs3")), NULL);
    iupSetObj(self->autorfs3, ObjNone, self, self->ui);
    IupSetCallback(self->autorfs3, "ACTION", (Icallback)screenshot_event_action);
    self->autowinfight = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_winfight")),
                                   NULL);
    iupSetObj(self->autowinfight, ObjNone, self, self->ui);
    IupSetCallback(self->autowinfight, "ACTION", (Icallback)screenshot_event_action);
    self->autodestroyed = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_destroyed")),
                                    NULL);
    iupSetObj(self->autodestroyed, ObjNone, self, self->ui);
    IupSetCallback(self->autodestroyed, "ACTION", (Icallback)screenshot_event_action);
    self->autogameover = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_gameover")),
                                   NULL);
    iupSetObj(self->autogameover, ObjNone, self, self->ui);
    IupSetCallback(self->autogameover, "ACTION", (Icallback)screenshot_event_action);
    self->autovictory = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_victory")),
                                  NULL);
    iupSetObj(self->autovictory, ObjNone, self, self->ui);
    IupSetCallback(self->autovictory, "ACTION", (Icallback)screenshot_event_action);
    self->autocredits = IupToggle(langGetC(self->ss, langGetC(self->ss, "screenshot_credits")),
                                  NULL);
    iupSetObj(self->autocredits, ObjNone, self, self->ui);
    IupSetCallback(self->autocredits, "ACTION", (Icallback)screenshot_event_action);

    Ihandle* autobox = IupGridBox(self->autoach,
                                  self->autorfs1,
                                  self->autowinfight,
                                  self->autorfs2,
                                  self->autodestroyed,
                                  self->autorfs3,
                                  self->autovictory,
                                  self->autogameover,
                                  self->autocredits,
                                  NULL);
    IupSetAttribute(autobox, "NUMDIV", "2");
    IupSetAttribute(autobox, "SIZELIN", "-1");
    IupSetAttribute(autobox, "CMARGIN", "0x0");
    IupSetAttribute(autobox, "CGAP", "1");

    Ihandle* thevbox = IupVbox(self->fmtradio,
                               fnamevbox,
                               fnamevarlistlabel,
                               toggleblock,
                               manspc,
                               mansep,
                               self->manlbl,
                               self->mloc->h,
                               autospc,
                               autosep,
                               autolbl,
                               autobox,
                               self->aloc->h,
                               IupFill(),
                               NULL);
    IupSetAttribute(thevbox, "CMARGIN", "6x6");
    IupSetAttribute(thevbox, "CGAP", "6");

    self->h = IupBackgroundBox(thevbox);

    iupLoadImage(self->ss, _S"IMAGE_SCREENSHOT", _S"svg", _S"subspace:/picture.svg", list);

    strDestroy(&temp);
    return true;
}

static void updateHotkey(ScreenshotPage* self)
{
    string kbname = 0, temp = 0;
    sskbname(self->ss, &kbname);
    strFormat(&temp, langGet(self->ss, _S"screenshot_manual"), stvar(string, kbname));
    strDestroy(&kbname);
    IupSetStrAttribute(self->manlbl, "TITLE", strC(temp));
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool ScreenshotPage_update(_In_ ScreenshotPage* self)
{
    updateHotkey(self);
    SSDNode* sets = self->feature->settings;

    string temp = 0;
    ssdStringOutD(sets, _S"filename", &temp, SCREENSHOTNAME_DEFAULT);
    pathToPlatform(&temp, temp);
    IupSetStrAttribute(self->namefmttext, "VALUE", strC(temp));

    ssdStringOutD(sets, _S"format", &temp, _S"png");
    if (strEq(temp, _S"jpeg"))
        IupSetAttributeHandle(self->fmtradio, "VALUE", self->fmtjpg);
    else
        IupSetAttributeHandle(self->fmtradio, "VALUE", self->fmtpng);
    screenshotpageUpdateFormatActive(self, temp);

    IupSetAttribute(self->sound, "VALUE", ssdVal(bool, sets, _S"sound", true) ? "ON" : "OFF");
    IupSetAttribute(self->hidemouse, "VALUE", ssdVal(bool, sets, _S"hidemouse", true) ? "ON" : "OFF");
    IupSetAttribute(self->hidepause,
                    "VALUE",
                    ssdVal(bool, sets, _S"hidepause", false) ? "ON" : "OFF");
    IupSetAttribute(self->hideinfoblock,
                    "VALUE",
                    ssdVal(bool, sets, _S"hideinfoblock", false) ? "ON" : "OFF");
    IupSetAttribute(self->metadata, "VALUE", ssdVal(bool, sets, _S"metadata", true) ? "ON" : "OFF");

    int quality = ssdVal(int32, self->feature->settings, _S"jpegquality", 95);
    IupSetfAttribute(self->jpgqvalnum, "VALUE", "%d", quality);
    IupSetInt(self->jpgqval, "VALUE", quality);

    int events = ssdVal(int32, sets, _S"events", 0);
    IupSetAttribute(self->autoach, "VALUE", (events & SSEvent_Ach) ? "ON" : "OFF");
    IupSetAttribute(self->autorfs1, "VALUE", (events & SSEvent_RFS1) ? "ON" : "OFF");
    IupSetAttribute(self->autorfs2, "VALUE", (events & SSEvent_RFS2) ? "ON" : "OFF");
    IupSetAttribute(self->autorfs3, "VALUE", (events & SSEvent_RFS3) ? "ON" : "OFF");
    IupSetAttribute(self->autowinfight, "VALUE", (events & SSEvent_WinFight) ? "ON" : "OFF");
    IupSetAttribute(self->autodestroyed, "VALUE", (events & SSEvent_Destroyed) ? "ON" : "OFF");
    IupSetAttribute(self->autogameover, "VALUE", (events & SSEvent_GameOver) ? "ON" : "OFF");
    IupSetAttribute(self->autovictory, "VALUE", (events & SSEvent_Victory) ? "ON" : "OFF");
    IupSetAttribute(self->autocredits, "VALUE", (events & SSEvent_Credits) ? "ON" : "OFF");

    ssloccontrolsUpdate(self->mloc);
    ssloccontrolsUpdate(self->aloc);
    strDestroy(&temp);
    return true;
}

void ScreenshotPage_updateFormatActive(_In_ ScreenshotPage* self, _In_opt_ strref fmt)
{
    if (strEq(fmt, _S"jpeg")) {
        IupSetAttribute(self->jpgqval, "ACTIVE", "YES");
        IupSetAttribute(self->jpgqvalnum, "ACTIVE", "YES");
        IupSetAttribute(self->nameextlabel, "TITLE", ".jpg");
    } else {
        IupSetAttribute(self->jpgqval, "ACTIVE", "NO");
        IupSetAttribute(self->jpgqvalnum, "ACTIVE", "NO");
        IupSetAttribute(self->nameextlabel, "TITLE", ".png");
    }
}

extern bool SettingsPage_show(_In_ SettingsPage* self);   // parent
#define parent_show() SettingsPage_show((SettingsPage*)(self))
bool ScreenshotPage_show(_In_ ScreenshotPage* self)
{
    updateHotkey(self);
    return true;
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

    self->page   = page;
    self->ss     = page->ss;
    self->isauto = isauto;

    objInstInit(self);
    return self;
}

static int screenshot_loc_radio_action(Ihandle* ih, int state)
{
    SSLocControls* self = iupGetParentObj(SSLocControls, ih);
    if (!self)
        return IUP_IGNORE;
    SSDNode* sets;

    if (self->isauto)
        sets = ssdSubtree(self->page->feature->settings, _S"auto", SSD_Create_Hashtable);
    else
        sets = ssdSubtree(self->page->feature->settings, _S"manual", SSD_Create_Hashtable);
    if (!sets)
        return IUP_IGNORE;

    int loc = SSLOC_Global;
    if (ih == self->rundir)
        loc = SSLOC_Rundir;
    else if (ih == self->custom)
        loc = SSLOC_Custom;

    ssdSet(sets, _S"loc", true, stvar(int32, loc));
    objRelease(&sets);

    ssloccontrolsUpdateButtons(self, loc);
    return IUP_DEFAULT;
}

bool SSLocControls_make(_In_ SSLocControls* self)
{
    self->global = IupToggle(langGetC(self->ss, "screenshot_global"), NULL);
    setTip(self->global,
           langGet(self->ss, _S"screenshot_global_tip"),
           langGet(self->ss, _S"screenshot_global"),
           0);
    iupSetObj(self->global, ObjNone, self, self->ss->ui);
    IupSetCallback(self->global, "ACTION", (Icallback)screenshot_loc_radio_action);

    self->globalopen = IupButton(langGetC(self->ss, "screenshot_global_open"), NULL);
    IupSetAttribute(self->globalopen, "CPADDING", "6x1");

    self->rundir = IupToggle(langGetC(self->ss, "screenshot_rundir"), NULL);
    setTip(self->rundir,
           langGet(self->ss, _S"screenshot_rundir_tip"),
           langGet(self->ss, _S"screenshot_rundir"),
           0);
    iupSetObj(self->rundir, ObjNone, self, self->ss->ui);
    IupSetCallback(self->rundir, "ACTION", (Icallback)screenshot_loc_radio_action);

    self->custom = IupToggle(langGetC(self->ss, "screenshot_custom"), NULL);
    iupSetObj(self->custom, ObjNone, self, self->ss->ui);
    IupSetCallback(self->custom, "ACTION", (Icallback)screenshot_loc_radio_action);

    self->ctext = IupText(NULL);
    IupSetAttribute(self->ctext, "EXPAND", "HORIZONTAL");
    self->cbrowse = IupButton(langGetC(self->ss, "settings_ftl_browse"), NULL);
    IupSetAttribute(self->cbrowse, "CPADDING", "6x1");
    Ihandle* mgbl = IupHbox(self->global, self->globalopen, NULL);
    IupSetAttribute(mgbl, "CMARGIN", "0x0");
    IupSetAttribute(mgbl, "CGAP", "6");
    IupSetAttribute(mgbl, "ALIGNMENT", "ACENTER");
    Ihandle* mcust = IupHbox(self->custom, self->ctext, self->cbrowse, NULL);
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
    SSDNode* sets;

    if (self->isauto)
        sets = ssdSubtree(self->page->feature->settings, _S"auto", SSD_Create_Hashtable);
    else
        sets = ssdSubtree(self->page->feature->settings, _S"manual", SSD_Create_Hashtable);
    if (!sets)
        return false;

    int loc         = ssdVal(int32, sets, _S"loc", SSLOC_Global);
    bool runtracker = fregIsEnabled(self->ss->freg, _S"RunTracker");
    if (loc == SSLOC_Rundir && !runtracker)
        loc = SSLOC_Global;

    switch (loc) {
    case SSLOC_Custom:
        IupSetAttributeHandle(self->h, "VALUE", self->custom);
        break;
    case SSLOC_Rundir:
        IupSetAttributeHandle(self->h, "VALUE", self->rundir);
        break;
    case SSLOC_Global:
    default:
        IupSetAttributeHandle(self->h, "VALUE", self->global);
        loc = SSLOC_Global;   // for below tests in case this is hit by default
        break;
    }

    ssloccontrolsUpdateButtons(self, loc);
    IupSetAttribute(self->rundir, "ACTIVE", runtracker ? "YES" : "NO");

    objRelease(&sets);

    return true;
}

void SSLocControls_updateButtons(_In_ SSLocControls* self, int32 loc)
{
    IupSetAttribute(self->globalopen, "ACTIVE", (loc == SSLOC_Global) ? "YES" : "NO");
    IupSetAttribute(self->ctext, "ACTIVE", (loc == SSLOC_Custom) ? "YES" : "NO");
    IupSetAttribute(self->cbrowse, "ACTIVE", (loc == SSLOC_Custom) ? "YES" : "NO");
}

// Autogen begins -----
#include "feature/screenshot/screenshotpage.auto.inc"
// Autogen ends -------
