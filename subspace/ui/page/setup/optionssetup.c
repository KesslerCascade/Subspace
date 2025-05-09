// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "optionssetup.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "feature/featureregistry.h"
#include "gamemgr/gamemgr.h"
#include "ui/optionswin.h"
#include "ui/subspaceui.h"
#include "ui/util/iuploadimage.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed SetupPage* SetupPage_create(SubspaceUI* ui)
{
    SetupPage* self;
    self = objInstCreate(SetupPage);

    self->ui = ui;
    self->ss = ui->ss;
    self->visible = true;

    self->name = _S"setup";
    strDup(&self->title, langGet(self->ss, _S"options_setup"));
    strDup(&self->imgname, _S"IMAGE_WRENCH_SMALL_BLACK");
    objInstInit(self);
    return self;
}

static void setTip(Ihandle* ih, strref tip, strref title, int icon)
{
    bool useballoon = !osIsWine();   // balloon tooltips don't work well on wine

    IupSetAttribute(ih, "TIP", strC(tip));
    if (!strEmpty(title)) {
        IupSetAttribute(ih, "TIPBALLOON", useballoon ? "YES" : "NO");
        IupSetAttribute(ih, "TIPBALLOONTITLE", strC(title));
        IupSetInt(ih, "TIPBALLOONTITLEICON", icon);
    }
    IupSetInt(ih, "TIPDELAY", 30000);
}

static void setCompatState(SetupPage* gp, strref state)
{
    string tmp = 0;

    strConcat(&tmp, _S"options_ftl_compat_", state);
    IupSetAttribute(gp->ftlcompatlabel, "TITLE", langGetC(gp->ss, tmp));
    strNConcat(&tmp, _S"options_ftl_compat_", state, _S"_tip");
    setTip(gp->ftlcompatlabel, langGet(gp->ss, tmp), NULL, 0);
    setTip(gp->ftlcompatimg, langGet(gp->ss, tmp), NULL, 0);
    strConcat(&tmp, _S"IMAGE_COMPAT_", state);
    strUpper(&tmp);

    // guarantee that a copy still exists in memory since IMAGE only stores a pointer...
    strDup(&gp->compatimg, tmp);
    IupSetAttribute(gp->ftlcompatimg, "IMAGE", strC(gp->compatimg));
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
        IupSetAttributeHandle(imsg, "PARENTDIALOG", gp->ss->ui->options->win);
        IupSetAttribute(imsg, "BUTTONS", "YESNO");
        IupSetAttribute(imsg, "DIALOGTYPE", "QUESTION");
        IupSetAttribute(imsg, "TITLE", langGetC(gp->ss, _S"langconfirm_title"));
        IupSetAttribute(imsg, "VALUE", langGetC(gp->ss, _S"langconfirm"));
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
    IupSetAttributeHandle(idlg, "PARENTDIALOG", gp->ss->ui->options->win);
    IupSetAttribute(idlg, "DIALOGTYPE", "OPEN");
    IupSetAttribute(idlg, "FILTER", "*.exe");
    IupSetAttribute(idlg, "FILTERINFO", langGetC(gp->ss, _S"exe_files"));
    IupSetAttribute(idlg, "TITLE", langGetC(gp->ss, _S"ftl_browse_title"));

    IupPopup(idlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(idlg, "STATUS") == 0) {
        string ftlexe   = 0;
        const char* val = IupGetAttribute(idlg, "VALUE");
        IupSetAttribute(gp->ftlloctext, "VALUE", val);
        pathFromPlatform(&ftlexe, (strref)val);
        pathNormalize(&ftlexe);

        objRelease(&gp->validateinst);
        gp->validateinst = ginstCreate(gp->ss->gmgr, ftlexe, LAUNCH_VALIDATE);
        gmgrReg(gp->ss->gmgr, gp->validateinst);
        bool ret = ginstLaunch(gp->validateinst);

        IupSetAttribute(gp->ftlver, "TITLE", langGetC(gp->ss, _S"options_ftl_ver_unknown"));
        setCompatState(gp, _S"unknown");

        // failed to launch, don't track it in gamemgr
        if (!ret) {
            gmgrUnreg(gp->ss->gmgr, gp->validateinst);
            setCompatState(gp, _S"incompat");
            objRelease(&gp->validateinst);
        } else {
            gp->vpending = true;
        }

        strDestroy(&ftlexe);
    }

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
        ssdSet(gp->ss->settings, _S"ftl/saveoverride", true, stvar(string, _S"user/"));
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
    IupSetAttributeHandle(idlg, "PARENTDIALOG", gp->ss->ui->options->win);
    IupSetAttribute(idlg, "DIALOGTYPE", "DIR");
    IupSetAttribute(idlg, "TITLE", langGetC(gp->ss, _S"options_saveoverride_browse_title"));
    if (!strEmpty(startdir))
        IupSetAttribute(idlg, "DIRECTORY", strC(startdir));

    IupPopup(idlg, IUP_CENTER, IUP_CENTER);

    if (IupGetInt(idlg, "STATUS") == 0) {
        const char* val = IupGetAttribute(idlg, "VALUE");
        IupSetAttribute(gp->saveoverridetext, "VALUE", val);

        string ovrdir = 0;
        pathFromPlatform(&ovrdir, (strref)val);
        pathNormalize(&ovrdir);
        ssdSet(gp->ss->settings, _S"ftl/saveoverride", true, stvar(string, ovrdir));
        strDestroy(&ovrdir);
    }

    strDestroy(&startdir);

    return IUP_DEFAULT;
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
            IupSetAttributeId(langselect, "", i + 1, strC(self->langnames.a[i]));

            strref langid = hteVal(languages_rev, strref, elem);
            saPush(&self->langids, strref, langid);
        }
    }

    htDestroy(&languages);
    htDestroy(&languages_rev);
}

extern bool OptionsPage_make(_In_ OptionsPage* self, Ihandle* list);   // parent
#define parent_make(list) OptionsPage_make((OptionsPage*)(self), list)
bool SetupPage_make(_In_ SetupPage* self, Ihandle* list)
{
    self->langselect = IupList(NULL);
    IupSetAttribute(self->langselect, "DROPDOWN", "YES");

    fillLanguages(self, self->langselect);

    iupSetObj(self->langselect, ObjNone, self, self->ui);
    IupSetCallback(self->langselect, "ACTION", (Icallback)langselect_change);

    Ihandle* langhbox = IupHbox(IupLabel(langGetC(self->ss, _S"options_lang")),
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
    self->ftlcompatlabel = IupLabel(langGetC(self->ss, _S"options_ftl_compat_unknown"));
    self->ftlcompatimg   = IupLabel("");
    IupSetAttribute(self->ftlcompatimg, "IMAGE", "IMAGE_COMPAT_UNKNOWN");
    Ihandle* ftlcompathbox = IupHbox(self->ftlcompatimg, self->ftlcompatlabel, NULL);
    IupSetAttribute(ftlcompathbox, "CGAP", "1");
    IupSetAttribute(ftlcompathbox, "CMARGIN", "3x1");

    setTip(self->ftlcompatimg, langGet(self->ss, _S"options_ftl_compat_unknown_tip"), NULL, 0);
    setTip(self->ftlcompatlabel, langGet(self->ss, _S"options_ftl_compat_unknown_tip"), NULL, 0);

    Ihandle* browsebtn = IupButton(langGetC(self->ss, _S"options_ftl_browse"), NULL);
    IupSetAttribute(browsebtn, "CPADDING", "6x1");
    iupSetObj(browsebtn, ObjNone, self, self->ui);
    IupSetCallback(browsebtn, "ACTION", (Icallback)browseforftl);

    Ihandle* ftlloclabel = IupLabel(langGetC(self->ss, _S"options_ftl_loc"));
    IupSetAttribute(ftlloclabel, "FONT", "Helvetica, Bold 10");
    Ihandle* ftlloclonglabel = IupLabel(langGetC(self->ss, _S"options_ftl_loc_long"));
    IupSetAttribute(ftlloclonglabel, "SIZE", "1x32");
    IupSetAttribute(ftlloclonglabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(ftlloclonglabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(ftlloclonglabel, "WORDWRAP", "YES");

    Ihandle* ftlhbox = IupHbox(self->ftlloctext, ftlcompathbox, browsebtn, NULL);
    IupSetAttribute(ftlhbox, "ALIGNMENT", "ACENTER");

    Ihandle* ftlverlabel = IupLabel(langGetC(self->ss, _S"options_ftl_ver"));
    self->ftlver         = IupLabel(langGetC(self->ss, _S"options_ftl_ver_unknown"));

    Ihandle* ftlverhbox = IupHbox(ftlverlabel, self->ftlver, NULL);

    self->saveoverridecheck = IupToggle(langGetC(self->ss, _S"options_saveoverride"), NULL);
    setTip(self->saveoverridecheck,
           langGet(self->ss, _S"options_saveoverride_tip"),
           langGet(self->ss, _S"options_saveoverride"),
           1);
    iupSetObj(self->saveoverridecheck, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverridecheck, "ACTION", (Icallback)saveovrchange);

    self->saveoverrideusercheck = IupToggle(langGetC(self->ss, _S"options_saveoverride_user"), NULL);
    setTip(self->saveoverrideusercheck,
           langGet(self->ss, _S"options_saveoverride_user_tip"),
           langGet(self->ss, _S"options_saveoverride_user"),
           1);
    iupSetObj(self->saveoverrideusercheck, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverrideusercheck, "ACTION", (Icallback)saveovruserchange);
    IupSetAttribute(self->saveoverrideusercheck, "FLOATING", "YES");
    IupSetAttribute(self->saveoverrideusercheck, "VISIBLE", "NO");

    Ihandle* saveovrlabel  = IupLabel(langGetC(self->ss, _S"options_saveoverride_folder"));
    self->saveoverridetext = IupText(NULL);
    IupSetAttribute(self->saveoverridetext, "EXPAND", "HORIZONTAL");
    iupSetObj(self->saveoverridetext, ObjNone, self, self->ui);
    IupSetCallback(self->saveoverridetext, "ACTION", (Icallback)saveovrtextchange);

    Ihandle* saveovrbrowse = IupButton(langGetC(self->ss, _S"options_saveoverride_browse"), NULL);
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

    Ihandle* featuresexplain = IupLabel(langGetC(self->ss, _S"options_features_explainer"));
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
    return parent_make(list);
}

extern bool OptionsPage_update(_In_ OptionsPage* self);   // parent
#define parent_update() OptionsPage_update((OptionsPage*)(self))
bool SetupPage_update(_In_ SetupPage* self)
{
    for (int i = 0; i < saSize(self->langids); i++) {
        if (strEq(self->ss->langid, self->langids.a[i])) {
            IupSetInt(self->langselect, "VALUE", i + 1);
            break;
        }
    }

    if (self->validateinst) {
        GameInstState state = GI_Init;
        GameInst* inst      = self->validateinst;
        string ftlloc       = 0;

        withReadLock (&inst->lock) {
            state = inst->state;
            strDup(&ftlloc, inst->exepath);
        }

        // make sure the dialog box reflects the path of the exe being validated
        pathToPlatform(&ftlloc, ftlloc);
        IupSetAttribute(self->ftlloctext, "VALUE", strC(ftlloc));
        strDestroy(&ftlloc);

        if (state == GI_Validated) {
            if (inst->ver[0] > 0) {
                strFormat(&self->verstr,
                          _S"${int}.${int}.${int}",
                          stvar(int32, inst->ver[0]),
                          stvar(int32, inst->ver[1]),
                          stvar(int32, inst->ver[2]));
                IupSetAttribute(self->ftlver, "TITLE", strC(self->verstr));
            } else {
                IupSetAttribute(self->ftlver,
                                "TITLE",
                                langGetC(self->ss, _S"options_ftl_ver_unknown"));
                strDestroy(&self->verstr);
            }

            // check features

            int nmatch = 0;
            int ntotal = 0;
            foreach (hashtable, hti, self->ss->freg->features) {
                SubspaceFeature* feat = (SubspaceFeature*)htiVal(object, hti);
                if (!feat->optional) {
                    ntotal++;
                    if (htHasKey(inst->features, string, feat->name))
                        nmatch++;
                }
            }

            if (nmatch < ntotal) {
                setCompatState(self, _S"partial");
            } else {
                setCompatState(self, _S"full");
            }

            if (self->vpending) {
                // save this to settings
                ssdSet(self->ss->settings, _S"ftl/exe", true, stvar(string, inst->exepath));
                // cache validation status and version
                ssdSet(self->ss->settings,
                       _S"ftl/compat",
                       true,
                       stvar(string, (nmatch < ntotal) ? _S"partial" : _S"full"));
                ssdSet(self->ss->settings, _S"ftl/ver", true, stvar(string, self->verstr));
                subspaceUpdateUI(self->ss);

                self->vpending = false;
            }
        } else if (state == GI_Failed) {
            IupSetAttribute(self->ftlver, "TITLE", langGetC(self->ss, _S"options_ftl_ver_unknown"));
            setCompatState(self, _S"incompat");
        } else if (state == GI_Exited) {
            IupSetAttribute(self->ftlver, "TITLE", langGetC(self->ss, _S"options_ftl_ver_unknown"));
            setCompatState(self, _S"unknown");
            objRelease(&self->validateinst);
        }
        // otherwise it's still checking
    } else {
        string ftlloc = 0;
        ssdStringOut(self->ss->settings, _S"ftl/exe", &ftlloc);
        pathToPlatform(&ftlloc, ftlloc);
        IupSetAttribute(self->ftlloctext, "VALUE", strC(ftlloc));
        strDestroy(&ftlloc);

        if (ssdStringOut(self->ss->settings, _S"ftl/ver", &self->verstr)) {
            IupSetAttribute(self->ftlver, "TITLE", strC(self->verstr));
        } else {
            IupSetAttribute(self->ftlver, "TITLE", langGetC(self->ss, _S"options_ftl_ver_unknown"));
        }
        string compat = 0;
        if (ssdStringOut(self->ss->settings, _S"ftl/compat", &compat)) {
            setCompatState(self, compat);
            strDestroy(&compat);
        } else {
            setCompatState(self, _S"unknown");
        }
    }

    string overrideloc = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/saveoverride", &overrideloc)) {
        IupSetAttribute(self->saveoverridecheck, "VALUE", "ON");
        IupSetAttribute(self->saveoverrideusercheck, "FLOATING", "NO");
        IupSetAttribute(self->saveoverrideusercheck, "VISIBLE", "YES");
        if (strEq(overrideloc, _S"user/")) {
            IupSetAttribute(self->saveoverrideusercheck, "VALUE", "ON");
            IupSetAttribute(self->saveoverridehbox, "VISIBLE", "NO");
            IupSetAttribute(self->saveoverridehbox, "FLOATING", "YES");
        } else {
            IupSetAttribute(self->saveoverrideusercheck, "VALUE", "OFF");
            IupSetAttribute(self->saveoverridehbox, "FLOATING", "NO");
            IupSetAttribute(self->saveoverridehbox, "VISIBLE", "YES");
            pathToPlatform(&self->overrideloc, overrideloc);
            IupSetAttribute(self->saveoverridetext, "VALUE", strC(self->overrideloc));
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
    strDestroy(&self->compatimg);
    strDestroy(&self->verstr);
    strDestroy(&self->overrideloc);
    objRelease(&self->validateinst);
    // Autogen ends -------
}

// Autogen begins -----
#include "optionssetup.auto.inc"
// Autogen ends -------
