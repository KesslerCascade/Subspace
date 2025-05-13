// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "featurespage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <iupdraw.h>
#include "feature/featureregistry.h"
#include "ui/subspaceui.h"

_objinit_guaranteed bool FeaturesPage_init(_In_ FeaturesPage* self)
{
    saInit(&self->featpanels, ptr, 8);
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

_objfactory_guaranteed FeaturesPage* FeaturesPage_create(SubspaceUI* ui)
{
    FeaturesPage* self;
    self = objInstCreate(FeaturesPage);

    self->ui      = ui;
    self->ss      = ui->ss;

    self->name = _S"features";
    strDup(&self->title, langGet(self->ss, _S"settings_features"));
    strDup(&self->imgname, _S"IMAGE_PUZZLE_PIECE_SMALL");

    objInstInit(self);
    return self;
}

static void setPanelState(Ihandle* fpanel, bool available, bool enabled)
{
    int curavail   = IupGetInt(fpanel, "PANEL_AVAILABLE");
    int curenabled = IupGetInt(fpanel, "PANEL_ENABLED");
    int newavail   = available ? 1 : 0;
    int newenabled = enabled ? 1 : 0;

    if (curavail != newavail || curenabled != newenabled) {
        IupSetInt(fpanel, "PANEL_AVAILABLE", newavail);
        IupSetInt(fpanel, "PANEL_ENABLED", newenabled);
        IupUpdate(fpanel);
    }
}

static int panel_button(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
    if (button != IUP_BUTTON1 || pressed != 0)
        return IUP_DEFAULT;

    SubspaceFeature* feat = (SubspaceFeature*)IupGetAttribute(ih, "FEATURE");
    FeaturesPage* fpage   = iupGetParentObj(FeaturesPage, ih);
    IupSetAttribute(ih, "PANEL_FOCUS", "0");   // hover border shouldn't stick around just because
                                               // clicking gave it focus

    if (feat && fpage) {
        rwlockAcquireRead(&feat->lock);
        if (feat->available) {
            bool enabled;
            enabled = feat->enabled;
            rwlockReleaseRead(&feat->lock);

            featureEnable(feat, !enabled);
        } else {
            rwlockReleaseRead(&feat->lock);
        }
    }

    return IUP_DEFAULT;
}

static int panel_key(Ihandle* ih, int c, int press)
{
    if (!(press == 0 && c == K_SP))
        return IUP_DEFAULT;

    SubspaceFeature* feat = (SubspaceFeature*)IupGetAttribute(ih, "FEATURE");
    FeaturesPage* fpage   = iupGetParentObj(FeaturesPage, ih);

    if (feat && fpage) {
        rwlockAcquireRead(&feat->lock);
        if (feat->available) {
            bool enabled;
            enabled = feat->enabled;
            rwlockReleaseRead(&feat->lock);

            featureEnable(feat, !enabled);
        } else {
            rwlockReleaseRead(&feat->lock);
        }
    }

    return IUP_DEFAULT;
}

static const char* panelbg          = "255 255 255";
static const char* panelbgenabled   = "240 255 240";
static const char* panelbgunavail   = "200 200 200";
static const char* paneltext        = "0 0 0";
static const char* paneltextunavail = "96 96 96";
static const char* panelfont        = "Helvetica, 9";
static const char* panelfonttitle   = "Helvetica, Bold 9";
static const char* panelfontenabled = "Helvetica, Italic 8";
static const char* hovercolor       = "96 96 192";

#define iupROUND(_x) ((int)((_x) > 0 ? (_x) + 0.5 : (_x) - 0.5))
static int panel_draw(Ihandle* ih, float posx, float posy)
{
    Subspace* ss           = iupGetSubspace(ih);
    bool avail             = IupGetInt(ih, "PANEL_AVAILABLE");
    bool enabled           = IupGetInt(ih, "PANEL_ENABLED");
    bool focus             = IupGetInt(ih, "PANEL_FOCUS");
    bool hover             = IupGetInt(ih, "PANEL_HOVER");
    bool drawhover         = focus || hover;
    const char* paneltitle = IupGetAttribute(ih, "PANEL_TITLE");
    const char* paneldesc  = IupGetAttribute(ih, "PANEL_DESC");
    int w, h, charw, charh;

    if (!ss || !paneltitle || !paneldesc)
        return IUP_DEFAULT;

    IupDrawBegin(ih);
    IupDrawGetSize(ih, &w, &h);
    IupGetIntInt(ih, "CHARSIZE", &charw, &charh);

    int xmargin = iupROUND((3 * charw) / 4.0);
    int ymargin = iupROUND((3 * charh) / 8.0);
    int xgap    = iupROUND((3 * charw) / 4.0);
    int ygap    = iupROUND((3 * charh) / 8.0);

    // background
    IupSetAttribute(ih, "DRAWCOLOR", avail ? (enabled ? panelbgenabled : panelbg) : panelbgunavail);
    IupSetAttribute(ih, "DRAWSTYLE", "FILL");
    IupDrawRectangle(ih, 1, 1, w - 2, h - 2);

    // outer border
    IupSetAttribute(ih, "DRAWCOLOR", drawhover ? hovercolor : paneltext);
    IupSetAttribute(ih, "DRAWSTYLE", "STROKE");
    if (drawhover)
        IupSetInt(ih, "DRAWLINEWIDTH", iupROUND((3 * charw) / 4.0));
    IupDrawRectangle(ih, 0, 0, w - 1, h - 1);
    IupSetAttribute(ih, "DRAWLINEWIDTH", "1");

    IupSetAttribute(ih, "DRAWTEXTCLIP", "YES");
    IupSetAttribute(ih, "DRAWTEXTWRAP", "NO");

    int ew  = 0;   // enabled text width
    int ewm = 0;   // ew with margin
    int dummy;
    if (enabled) {
        IupSetAttribute(ih, "DRAWFONT", panelfontenabled);
        IupDrawGetTextSize(ih, langGetC(ss, _S"feature_enabled"), -1, &ew, &dummy);
        ew++;
        ewm = ew + 6;
    }

    IupSetAttribute(ih, "DRAWCOLOR", avail ? paneltext : paneltextunavail);
    IupSetAttribute(ih, "DRAWFONT", panelfonttitle);
    int th = 0;
    IupDrawGetTextSize(ih, paneltitle, -1, &dummy, &th);
    IupDrawText(ih,
                paneltitle,
                -1,
                2 + xmargin,
                2 + ymargin,
                w - ewm - xmargin * 2 - 4,
                h - ymargin * 2 - 4);

    if (enabled) {
        IupSetAttribute(ih, "DRAWFONT", panelfontenabled);
        IupDrawText(ih,
                    langGetC(ss, _S"feature_enabled"),
                    -1,
                    w - ew - xmargin - 2,
                    2 + ymargin,
                    ew,
                    h - ymargin * 2 - 4);
    }

    IupSetAttribute(ih, "DRAWFONT", panelfont);
    IupSetAttribute(ih, "DRAWTEXTWRAP", "YES");
    IupDrawText(ih,
                paneldesc,
                -1,
                2 + xmargin,
                2 + ymargin + th + ygap,
                w - xmargin * 2 - 4,
                h - th - ymargin * 2 - 4);

    IupDrawEnd(ih);

    return IUP_DEFAULT;
}

static int panel_getfocus(Ihandle* ih)
{
    IupSetInt(ih, "PANEL_FOCUS", 1);
    IupUpdate(ih);
    return IUP_DEFAULT;
}

static int panel_killfocus(Ihandle* ih)
{
    IupSetInt(ih, "PANEL_FOCUS", 0);
    IupUpdate(ih);
    return IUP_DEFAULT;
}

static int panel_enterwindow(Ihandle* ih)
{
    IupSetInt(ih, "PANEL_HOVER", 1);
    IupUpdate(ih);
    return IUP_DEFAULT;
}

static int panel_leavewindow(Ihandle* ih)
{
    IupSetInt(ih, "PANEL_HOVER", 0);
    IupUpdate(ih);
    return IUP_DEFAULT;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool FeaturesPage_make(_In_ FeaturesPage* self, Ihandle* list)
{
    Ihandle* vbox = IupVbox(NULL);
    IupSetAttribute(vbox, "CMARGIN", "6x6");
    IupSetAttribute(vbox, "CGAP", "4");

    Ihandle* instlabel = IupLabel(langGetC(self->ss, _S"settings_features_instruction"));
    IupSetAttribute(instlabel, "SIZE", "1x18");
    IupSetAttribute(instlabel, "EXPAND", "HORIZONTAL");
    IupSetAttribute(instlabel, "ALIGNMENT", "ALEFT:ATOP");
    IupSetAttribute(instlabel, "WORDWRAP", "YES");
    IupAppend(vbox, instlabel);

    Ihandle* instspc = IupSpace();
    IupSetAttribute(instspc, "SIZE", "1x1");
    IupAppend(vbox, instspc);

    foreach (hashtable, hti, self->ss->freg->features) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(object, hti);
        string sname          = 0;
        strConcat(&sname, _S"feature_", feat->name);
        strLower(&sname);

        Ihandle* cpanel = IupCanvas(NULL);
        IupSetAttribute(cpanel, "PANEL_TITLE", langGetC(self->ss, sname));
        strAppend(&sname, _S"_desc");
        IupSetAttribute(cpanel, "PANEL_DESC", langGetC(self->ss, sname));
        strDestroy(&sname);
        IupSetAttribute(cpanel, "BORDER", "NO");
        IupSetAttribute(cpanel, "EXPAND", "HORIZONTAL");
        IupSetAttribute(cpanel, "SIZE", "1x40");
        IupSetAttribute(cpanel, "FEATURE", (char*)feat);
        iupSetObj(cpanel, ObjNone, self, self->ui);

        IupSetCallback(cpanel, "ACTION", (Icallback)panel_draw);
        IupSetCallback(cpanel, "GETFOCUS_CB", (Icallback)panel_getfocus);
        IupSetCallback(cpanel, "KILLFOCUS_CB", (Icallback)panel_killfocus);
        IupSetCallback(cpanel, "ENTERWINDOW_CB", (Icallback)panel_enterwindow);
        IupSetCallback(cpanel, "LEAVEWINDOW_CB", (Icallback)panel_leavewindow);
        IupSetCallback(cpanel, "BUTTON_CB", (Icallback)panel_button);
        IupSetCallback(cpanel, "KEYPRESS_CB", (Icallback)panel_key);

        IupAppend(vbox, cpanel);

        saPush(&self->featpanels, ptr, cpanel);
    }

    IupAppend(vbox, IupFill());

    self->h = IupScrollBox(vbox);

    iupLoadImage(self->ss,
        _S"IMAGE_PUZZLE_PIECE_SMALL",
        _S"svg",
        _S"subspace:/puzzle-piece-small.svg",
        list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool FeaturesPage_update(_In_ FeaturesPage* self)
{
    int idx = 0;
    foreach (sarray, idx, Ihandle*, fpanel, self->featpanels) {
        SubspaceFeature* feat = (SubspaceFeature*)IupGetAttribute(fpanel, "FEATURE");

        if (feat) {
            bool avail, enabled;
            withReadLock (&feat->lock) {
                avail   = feat->available;
                enabled = feat->enabled;
            }

            setPanelState(fpanel, avail, enabled);
        }
    }

    return true;
}

void FeaturesPage_destroy(_In_ FeaturesPage* self)
{
    // Autogen begins -----
    saDestroy(&self->featpanels);
    // Autogen ends -------
}

// Autogen begins -----
#include "featurespage.auto.inc"
// Autogen ends -------
