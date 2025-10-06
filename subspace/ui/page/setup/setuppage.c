// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ui/page/setup/setuppage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "feature/featureregistry.h"
#include "gamemgr/gamemgr.h"
#include "ui/subspaceui.h"
#include "ui/util/iuploadimage.h"
#include "ui/win/settingswin.h"

_objfactory_guaranteed SetupPage* SetupPage_create(SubspaceUI* ui)
{
    SetupPage* self;
    self = objInstCreate(SetupPage);

    self->ui = ui;
    self->ss = ui->ss;

    self->name = _S"setup";
    strDup(&self->title, langGet(self->ss, _S"settings_setup"));
    strDup(&self->imgname, _S"IMAGE_WRENCH_SMALL_BLACK");
    objInstInit(self);
    return self;
}

static void setCompatState(SetupPage* gp, strref state)
{
    string tmp = 0;

    strConcat(&tmp, _S"settings_ftl_compat_", state);
    IupSetStrAttribute(gp->ftlcompatlabel, "TITLE", strC(langGet(gp->ss, tmp)));
    strNConcat(&tmp, _S"settings_ftl_compat_", state, _S"_tip");
    setTip(gp->ftlcompatlabel, langGet(gp->ss, tmp), NULL, 0);
    setTip(gp->ftlcompatimg, langGet(gp->ss, tmp), NULL, 0);
    strConcat(&tmp, _S"IMAGE_COMPAT_", state);
    strUpper(&tmp);

    IupSetStrAttribute(gp->ftlcompatimg, "IMAGE", strC(tmp));
    IupRefresh(gp->ftlcompatlabel);

    strDestroy(&tmp);
}

static int langselect_change(Ihandle* ih, char* text, int item, int state)
{
    if (state != 1)
        return IUP_DEFAULT;

    SetupPage* gp = iupGetParentObj(SetupPage, ih);

    if (gp && item > 0 && item <= saSize(gp->langids) &&
        !strEq(gp->langids.a[item - 1], gp->ss->langid)) {
        ssdSet(gp->ss->settings, _S"ui/lang", true, stvar(string, gp->langids.a[item - 1]));

        Ihandle* imsg = IupMessageDlg();
        IupSetAttributeHandle(imsg, "PARENTDIALOG", gp->ss->ui->settingsw->win);
        IupSetAttribute(imsg, "BUTTONS", "YESNO");
        IupSetAttribute(imsg, "DIALOGTYPE", "QUESTION");
        IupSetStrAttribute(imsg, "TITLE", langGetC(gp->ss, "langconfirm_title"));
        IupSetStrAttribute(imsg, "VALUE", langGetC(gp->ss, "langconfirm"));
        IupPopup(imsg, IUP_CENTER, IUP_CENTER);

        int resp = IupGetInt(imsg, "BUTTONRESPONSE");
        IupDestroy(imsg);

        if (resp == 1) {
            gp->ss->reloadui = true;
            eventSignal(&gp->ss->notify);
        }
    }

    return IUP_DEFAULT;
}

static int browseforftl(Ihandle* ih)
{
    SetupPage* gp = iupGetParentObj(SetupPage, ih);
    if (!gp)
        return IUP_IGNORE;

    Ihandle* idlg = IupFileDlg();
    IupSetAttributeHandle(idlg, "PARENTDIALOG", gp->ss->ui->settingsw->win);
    IupSetAttribute(idlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(idlg, "FILTER", "*.exe");
    IupSetStrAttribute(idlg, "FILTERINFO", langGetC(gp->ss, "exe_files"));
    IupSetStrAttribute(idlg, "TITLE", langGetC(gp->ss, "ftl_browse_title"));
    const char* curloc = IupGetAttribute(gp->ftlloctext, "VALUE");
    if (curloc && curloc[0])
        IupSetStrAttribute(idlg, "FILE", curloc);

    IupPopup(idlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(idlg, "STATUS") == 0) {
        string ftlexe   = 0;
        const char* val = IupGetAttribute(idlg, "VALUE");
        IupSetStrAttribute(gp->ftlloctext, "VALUE", val);
        pathFromPlatform(&ftlexe, (strref)val);
        pathNormalize(&ftlexe);

        gmgrValidate(gp->ss->gmgr, ftlexe, NULL);
        strDestroy(&ftlexe);
    }

    IupDestroy(idlg);

    return IUP_DEFAULT;
}

static int saveovrchange(Ihandle* ih, int state)
{
    SetupPage* gp = iupGetParentObj(SetupPage, ih);
    if (!gp)
        return IUP_IGNORE;

    if (state == 1) {
        IupSetAttribute(gp->saveoverrideusercheck, "VALUE", "OFF");
        IupSetAttribute(gp->saveoverrideusercheck, "FLOATING", "NO");
        IupSetAttribute(gp->saveoverrideusercheck, "VISIBLE", "YES");
        IupSetAttribute(gp->saveoverridetext, "VALUE", "");
        IupSetAttribute(gp->saveoverridehbox, "FLOATING", "NO");
        IupSetAttribute(gp->saveoverridehbox, "VISIBLE", "YES");
    } else {
        ssdRemove(gp->ss->settings, _S"ftl/saveoverride");
        IupSetAttribute(gp->saveoverrideusercheck, "VISIBLE", "NO");
        IupSetAttribute(gp->saveoverrideusercheck, "FLOATING", "YES");
        IupSetAttribute(gp->saveoverridehbox, "VISIBLE", "NO");
        IupSetAttribute(gp->saveoverridehbox, "FLOATING", "YES");
    }
    IupRefresh(ih);
    return IUP_DEFAULT;
}

static int saveovruserchange(Ihandle* ih, int state)
{
    SetupPage* gp = iupGetParentObj(SetupPage, ih);
    if (!gp)
        return IUP_IGNORE;

    if (state == 1) {
        ssdSet(gp->ss->settings, _S"ftl/saveoverride", true, stvar(string, _S"[User]"));
        IupSetAttribute(gp->saveoverridehbox, "VISIBLE", "NO");
        IupSetAttribute(gp->saveoverridehbox, "FLOATING", "YES");
    } else {
        ssdRemove(gp->ss->settings, _S"ftl/saveoverride");
        IupSetAttribute(gp->saveoverridetext, "VALUE", "");
        IupSetAttribute(gp->saveoverridehbox, "FLOATING", "NO");
        IupSetAttribute(gp->saveoverridehbox, "VISIBLE", "YES");
    }
    IupRefresh(ih);
    return IUP_DEFAULT;
}

static int saveovrtextchange(Ihandle* ih, int c, char* new_value)
{
    SetupPage* gp = iupGetParentObj(SetupPage, ih);
    if (!gp)
        return IUP_IGNORE;

    string ovrdir = 0;
    pathFromPlatform(&ovrdir, (strref)new_value);
    pathNormalize(&ovrdir);
    ssdSet(gp->ss->settings, _S"ftl/saveoverride", true, stvar(string, ovrdir));
    strDestroy(&ovrdir);

    return IUP_DEFAULT;
}

static int browseforsaveovr(Ihandle* ih)
{
    SetupPage* gp = iupGetParentObj(SetupPage, ih);
    if (!gp)
        return IUP_IGNORE;

    Ihandle* idlg   = IupFileDlg();
    string startdir = 0;
    if (ssdStringOut(gp->ss->settings, _S"ftl/exe", &startdir)) {
        pathParent(&startdir, startdir);
        pathToPlatform(&startdir, startdir);
    }
    IupSetAttributeHandle(idlg, "PARENTDIALOG", gp->ss->ui->settingsw->win);
    IupSetAttribute(idlg, "DIALOGTYPE", "DIR");
    IupSetStrAttribute(idlg, "TITLE", langGetC(gp->ss, "settings_saveoverride_browse_title"));
    if (!strEmpty(startdir))
        IupSetStrAttribute(idlg, "DIRECTORY", strC(startdir));

    IupPopup(idlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(idlg, "STATUS") == 0) {
        const char* val = IupGetAttribute(idlg, "VALUE");
        IupSetStrAttribute(gp->saveoverridetext, "VALUE", val);

        string ovrdir = 0;
        pathFromPlatform(&ovrdir, (strref)val);
        pathNormalize(&ovrdir);
        ssdSet(gp->ss->settings, _S"ftl/saveoverride", true, stvar(string, ovrdir));
        strDestroy(&ovrdir);
    }

    IupDestroy(idlg);
    strDestroy(&startdir);

    return IUP_DEFAULT;
}

void SetupPage_onValidateStart(_In_ SetupPage* self, _In_opt_ strref event, stvlist* params)
{
    IupSetStrAttribute(self->ftlver, "TITLE", langGetC(self->ss, "settings_ftl_ver_unknown"));
    setCompatState(self, _S"unknown");
}

void SetupPage_onValidateFinish(_In_ SetupPage* self, _In_opt_ strref event, stvlist* params)
{
    GameInst* inst = stvlNextObj(params, GameInst);
    if (!inst)
        return;

    GameInstState state = ginstGetState(inst);
    // for failed and exited state, we want the dialog to show the results even though the settings
    // weren't updated
    if (state == GI_Failed) {
        IupSetStrAttribute(self->ftlver, "TITLE", langGetC(self->ss, "settings_ftl_ver_unknown"));
        setCompatState(self, _S"incompat");
    } else if (state == GI_Exited) {
        IupSetStrAttribute(self->ftlver, "TITLE", langGetC(self->ss, "settings_ftl_ver_unknown"));
        setCompatState(self, _S"unknown");
    } else {
        // otherwise just read the results from settings to avoid duplicating code
        setuppageUpdateCompatState(self);
    }
}

static void fillLanguages(SetupPage* self, Ihandle* langselect)
{
    hashtable languages, languages_rev;
    htInit(&languages, string, string, 32);
    htInit(&languages_rev, string, string, 32);
    langList(self->ss, &languages);

    foreach (hashtable, hti, languages) {
        htInsert(&languages_rev, strref, htiVal(strref, hti), strref, htiKey(strref, hti));
        saPush(&self->langnames, strref, htiVal(strref, hti));
    }

    for (int i = 0; i < saSize(self->langnames); i++) {
        htelem elem = htFind(languages_rev, string, self->langnames.a[i], none, NULL);
        if (elem) {
            IupSetStrAttributeId(langselect, "", i + 1, strC(self->langnames.a[i]));

            strref langid = hteVal(languages_rev, strref, elem);
            saPush(&self->langids, strref, langid);
        }
    }

    htDestroy(&languages);
    htDestroy(&languages_rev);
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool SetupPage_make(_In_ SetupPage* self, Ihandle* list)
{
    self->langselect = IupList(NULL);
    IupSetAttribute(self->langselect, "DROPDOWN", "YES");

    fillLanguages(self, self->langselect);

    iupSetObj(self->langselect, ObjNone, self, self->ui);
    IupSetCallback(self->langselect, "ACTION", (Icallback)langselect_change);

    Ihandle* langhbox = IupHbox(IupLabel(langGetC(self->ss, "settings_lang")),
                                self->langselect,
                                IupFill(),
                                NULL);
    IupSetAttribute(langhbox, "ALIGNMENT", "ACENTER");

    Ihandle* spacer1 = IupSpace();
    IupSetAttribute(spacer1, "SIZE", "1x6");
    Ihandle* sep1 = IupLabel("");
    IupSetAttribute(sep1, "SEPARATOR", "HORIZONTAL");

    self->ftlloctext = IupText(NULL);
    IupSetAttribute(self->ftlloctext, "EXPAND", "HORIZONTAL");
    IupSetAttribute(self->ftlloctext, "ACTIVE", "NO");
    self->ftlcompatlabel = IupLabel(langGetC(self->ss, "settings_ftl_compat_unknown"));
    self->ftlcompatimg   = IupLabel("");
    IupSetAttribute(self->ftlcompatimg, "IMAGE", "IMAGE_COMPAT_UNKNOWN");
    Ihandle* ftlcompathbox = IupHbox(self->ftlcompatimg, self->ftlcompatlabel, NULL);
    IupSetAttribute(ftlcompathbox, "CGAP", "1");
    IupSetAttribute(ftlcompathbox, "CMARGIN", "3x1");

    setTip(self->ftlcompatimg, langGet(self->ss, _S"settings_ftl_compat_unknown_tip"), NULL, 0);
    setTip(self->ftlcompatlabel, langGet(self->ss, _S"settings_ftl_compat_unknown_tip"), NULL, 0);

    Ihandle* browsebtn = IupButton(langGetC(self->ss, "settings_ftl_browse"), NULL);
    IupSetAttribute(browsebtn, "CPADDING", "6x1");
    iupSetObj(browsebtn, ObjNone, self, self->ui);
    IupSetCallback(browsebtn, "ACTION", (Icallback)browseforftl);

    Ihandle* ftlloclabel = IupLabel(langGetC(self->ss, "settings_ftl_loc"));
    IupSetAttribute(ftlloclabel, "FONT", "Helvetica, Bold 10");
    Ihandle* ftlloclonglabel = IupLabel(langGetC(self->ss, "settings_ftl_loc_long"));
    IupSetAttribute(ftlloclonglabel, "SIZE", "1x32");
    IupSetAttribute(ftlloclonglabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(ftlloclonglabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(ftlloclonglabel, "WORDWRAP", "YES");

    Ihandle* ftlhbox = IupHbox(self->ftlloctext, ftlcompathbox, browsebtn, NULL);
    IupSetAttribute(ftlhbox, "ALIGNMENT", "ACENTER");

    Ihandle* ftlverlabel = IupLabel(langGetC(self->ss, "settings_ftl_ver"));
    self->ftlver         = IupLabel(langGetC(self->ss, "settings_ftl_ver_unknown"));

    Ihandle* ftlverhbox = IupHbox(ftlverlabel, self->ftlver, NULL);

    self->saveoverridecheck = IupToggle(langGetC(self->ss, "settings_saveoverride"), NULL);
    setTip(self->saveoverridecheck,
           langGet(self->ss, _S"settings_saveoverride_tip"),
           langGet(self->ss, _S"settings_saveoverride"),
           1);
    iupSetObj(self->saveoverridecheck, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverridecheck, "ACTION", (Icallback)saveovrchange);

    self->saveoverrideusercheck = IupToggle(langGetC(self->ss, "settings_saveoverride_user"), NULL);
    setTip(self->saveoverrideusercheck,
           langGet(self->ss, _S"settings_saveoverride_user_tip"),
           langGet(self->ss, _S"settings_saveoverride_user"),
           1);
    iupSetObj(self->saveoverrideusercheck, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverrideusercheck, "ACTION", (Icallback)saveovruserchange);
    IupSetAttribute(self->saveoverrideusercheck, "FLOATING", "YES");
    IupSetAttribute(self->saveoverrideusercheck, "VISIBLE", "NO");

    Ihandle* saveovrlabel  = IupLabel(langGetC(self->ss, "settings_saveoverride_folder"));
    self->saveoverridetext = IupText(NULL);
    IupSetAttribute(self->saveoverridetext, "EXPAND", "HORIZONTAL");
    iupSetObj(self->saveoverridetext, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverridetext, "ACTION", (Icallback)saveovrtextchange);

    Ihandle* saveovrbrowse = IupButton(langGetC(self->ss, "settings_saveoverride_browse"), NULL);
    IupSetAttribute(saveovrbrowse, "CPADDING", "6x1");
    iupSetObj(saveovrbrowse, ObjNone, self, self->ui);
    IupSetCallback(saveovrbrowse, "ACTION", (Icallback)browseforsaveovr);

    self->saveoverridehbox = IupHbox(saveovrlabel, self->saveoverridetext, saveovrbrowse, NULL);
    IupSetAttribute(self->saveoverridehbox, "FLOATING", "YES");
    IupSetAttribute(self->saveoverridehbox, "VISIBLE", "NO");

    Ihandle* spacer2 = IupSpace();
    IupSetAttribute(spacer2, "SIZE", "1x4");

    Ihandle* spacer3 = IupSpace();
    IupSetAttribute(spacer3, "SIZE", "1x8");

    Ihandle* featuressep = IupLabel("");
    IupSetAttribute(featuressep, "SEPARATOR", "HORIZONTAL");

    Ihandle* featuresexplain = IupLabel(langGetC(self->ss, "settings_features_explainer"));
    IupSetAttribute(featuresexplain, "SIZE", "1x1");
    IupSetAttribute(featuresexplain, "EXPAND", "YES");
    IupSetAttribute(featuresexplain, "WORDWRAP", "YES");
    IupSetAttribute(featuresexplain, "ALIGNMENT", "ALEFT:ATOP");

    Ihandle* thevbox = IupVbox(langhbox,
                               spacer1,
                               sep1,
                               ftlloclabel,
                               ftlloclonglabel,
                               ftlhbox,
                               ftlverhbox,
                               spacer2,
                               self->saveoverridecheck,
                               self->saveoverrideusercheck,
                               self->saveoverridehbox,
                               spacer3,
                               featuressep,
                               featuresexplain,
                               IupFill(),
                               NULL);
    IupSetAttribute(thevbox, "CMARGIN", "0x0");
    IupSetAttribute(thevbox, "CGAP", "4");
    Ihandle* vboxwrapper = IupVbox(thevbox, NULL);
    IupSetAttribute(vboxwrapper, "CMARGIN", "6x6");
    self->h = IupBackgroundBox(vboxwrapper);

    iupLoadImage(self->ss,
                 _S"IMAGE_WRENCH_SMALL_BLACK",
                 _S"svg",
                 _S"subspace:/wrench-small-black.svg",
                 list);
    iupLoadImage(self->ss,
                 _S"IMAGE_COMPAT_UNKNOWN",
                 _S"svg",
                 _S"subspace:/compat-unknown.svg",
                 self->ftlcompatimg);
    iupLoadImage(self->ss,
                 _S"IMAGE_COMPAT_FULL",
                 _S"svg",
                 _S"subspace:/compat-full.svg",
                 self->ftlcompatimg);
    iupLoadImage(self->ss,
                 _S"IMAGE_COMPAT_PARTIAL",
                 _S"svg",
                 _S"subspace:/compat-partial.svg",
                 self->ftlcompatimg);
    iupLoadImage(self->ss,
                 _S"IMAGE_COMPAT_INCOMPAT",
                 _S"svg",
                 _S"subspace:/compat-incompat.svg",
                 self->ftlcompatimg);

    ssuiListen(self->ui, self, SetupPage_onValidateStart, _S"Validate_Start");
    ssuiListen(self->ui, self, SetupPage_onValidateFinish, _S"Validate_Finish");

    return parent_make(list);
}

void SetupPage_updateCompatState(_In_ SetupPage* self)
{
    string ftlloc = 0, verstr = 0;
    ssdStringOut(self->ss->settings, _S"ftl/exe", &ftlloc);
    pathToPlatform(&ftlloc, ftlloc);
    IupSetStrAttribute(self->ftlloctext, "VALUE", strC(ftlloc));

    if (ssdStringOut(self->ss->settings, _S"ftl/ver", &verstr)) {
        IupSetStrAttribute(self->ftlver, "TITLE", strC(verstr));
    } else {
        IupSetStrAttribute(self->ftlver, "TITLE", langGetC(self->ss, "settings_ftl_ver_unknown"));
    }
    string compat = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/compat", &compat)) {
        setCompatState(self, compat);
        strDestroy(&compat);
    } else {
        setCompatState(self, _S"unknown");
    }

    strDestroy(&ftlloc);
    strDestroy(&verstr);
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool SetupPage_update(_In_ SetupPage* self)
{
    for (int i = 0; i < saSize(self->langids); i++) {
        if (strEq(self->ss->langid, self->langids.a[i])) {
            IupSetInt(self->langselect, "VALUE", i + 1);
            break;
        }
    }

    setuppageUpdateCompatState(self);

    string overrideloc = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/saveoverride", &overrideloc)) {
        IupSetAttribute(self->saveoverridecheck, "VALUE", "ON");
        IupSetAttribute(self->saveoverrideusercheck, "FLOATING", "NO");
        IupSetAttribute(self->saveoverrideusercheck, "VISIBLE", "YES");
        if (strEq(overrideloc, _S"[User]")) {
            IupSetAttribute(self->saveoverrideusercheck, "VALUE", "ON");
            IupSetAttribute(self->saveoverridehbox, "VISIBLE", "NO");
            IupSetAttribute(self->saveoverridehbox, "FLOATING", "YES");
        } else {
            IupSetAttribute(self->saveoverrideusercheck, "VALUE", "OFF");
            IupSetAttribute(self->saveoverridehbox, "FLOATING", "NO");
            IupSetAttribute(self->saveoverridehbox, "VISIBLE", "YES");
            pathToPlatform(&overrideloc, overrideloc);
            IupSetStrAttribute(self->saveoverridetext, "VALUE", strC(overrideloc));
        }
    } else {
        IupSetAttribute(self->saveoverridecheck, "VALUE", "OFF");
        IupSetAttribute(self->saveoverrideusercheck, "FLOATING", "YES");
        IupSetAttribute(self->saveoverrideusercheck, "VISIBLE", "NO");
        IupSetAttribute(self->saveoverridehbox, "VISIBLE", "YES");
        IupSetAttribute(self->saveoverridehbox, "VISIBLE", "NO");
    }
    IupRefresh(self->saveoverridehbox);

    return parent_update();
}

_objinit_guaranteed bool SetupPage_init(_In_ SetupPage* self)
{
    // Autogen begins -----
    saInit(&self->langids, string, 1);
    saInit(&self->langnames, string, 1, SA_Sorted);
    return true;
    // Autogen ends -------
}

void SetupPage_destroy(_In_ SetupPage* self)
{
    // Autogen begins -----
    saDestroy(&self->langids);
    saDestroy(&self->langnames);
    // Autogen ends -------
}

// Autogen begins -----
// clang-format off
#include "ui/page/setup/setuppage.auto.inc"
// clang-format on
// Autogen ends -------
