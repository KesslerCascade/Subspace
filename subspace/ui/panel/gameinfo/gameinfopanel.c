// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gameinfopanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gamemgr.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed GameInfoPanel* GameInfoPanel_create(SubspaceUI* ui)
{
    GameInfoPanel* self;
    self = objInstCreate(GameInfoPanel);
    self->ss = ui->ss;
    self->ui = ui;
    strDup(&self->title, langGet(self->ss, _S"panel_gameinfo"));
    self->name = _S"gameinfo";
    objInstInit(self);
    return self;
}

static int playbtn_action(Ihandle* ih)
{
    Subspace* ss = iupGetSubspace(ih);
    if (!ss)
        return IUP_IGNORE;

    gmgrLaunchGame(ss->gmgr, LAUNCH_PLAY, NULL);
    return IUP_DEFAULT;
}

static void makeNotRunning(GameInfoPanel* self)
{
    Ihandle* nrlabel1 = IupFlatLabel(langGetC(self->ss, _S"ftl_notrunning"));
    IupSetAttribute(nrlabel1, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(nrlabel1, "FGCOLOR", "255 255 255");
    IupSetAttribute(nrlabel1, "EXPAND", "HORIZONTAL");
    IupSetAttribute(nrlabel1, "ALIGNMENT", "ACENTER");

    Ihandle* hplabel1 = IupFlatLabel(langGetC(self->ss, _S"ftl_howtoplay1"));
    IupSetAttribute(hplabel1, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel1, "FGCOLOR", "255 255 255");
    Ihandle* hplabel2 = IupFlatLabel(langGetC(self->ss, _S"ftl_howtoplay2"));
    IupSetAttribute(hplabel2, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel2, "FGCOLOR", "255 255 255");
    Ihandle* hplabel3 = IupFlatLabel(langGetC(self->ss, _S"ftl_howtoplay3"));
    IupSetAttribute(hplabel3, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel3, "FGCOLOR", "255 255 255");
    self->playbtn = IupFlatButton(langGetC(self->ss, _S"play"));
    IupSetAttribute(self->playbtn, "BGCOLOR", "85 44 92");
    IupSetAttribute(self->playbtn, "HLCOLOR", "139 73 131");
    IupSetAttribute(self->playbtn, "PSCOLOR", "48 25 52");
    IupSetAttribute(self->playbtn, "BORDERCOLOR", "139 73 131");
    IupSetAttribute(self->playbtn, "BORDERHLCOLOR", "174 101 165");
    IupSetAttribute(self->playbtn, "BORDERPSCOLOR", "85 44 92");
    IupSetAttribute(self->playbtn, "FGCOLOR", "255 255 255");
    IupSetAttribute(self->playbtn, "IMAGE", "IMAGE_PLAY_SMALL");
    IupSetAttribute(self->playbtn, "FONT", "Helvetica, 10");
    IupSetAttribute(self->playbtn, "CPADDING", "4x1");
    IupSetAttribute(self->playbtn, "CSPACING", "3");
    IupSetAttribute(self->playbtn, "FOCUSFEEDBACK", "No");
    iupLoadImage(self->ss, _S"IMAGE_PLAY_SMALL", _S"svg", _S"subspace:/play-small.svg", self->playbtn);
    iupSetObj(self->playbtn, ObjNone, self, self->ui);
    IupSetCallback(self->playbtn, "FLAT_ACTION", playbtn_action);

    Ihandle* playhbox = IupHbox(hplabel1, self->playbtn, hplabel2, NULL);
    IupSetAttribute(playhbox, "CGAP", "3");

    self->notrunning = IupVbox(nrlabel1, playhbox, hplabel3, IupFill(), NULL);
    IupSetAttribute(self->notrunning, "ALIGNMENT", "ACENTER");
    IupSetAttribute(self->notrunning, "CGAP", "10");
    IupSetAttribute(self->notrunning, "NCMARGIN", "8x8");
}

static void makeLoading(GameInfoPanel* self)
{
    Ihandle* loadinglbl = IupFlatLabel(langGetC(self->ss, _S"ftl_loading"));
    IupSetAttribute(loadinglbl, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(loadinglbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(loadinglbl, "EXPAND", "HORIZONTAL");
    IupSetAttribute(loadinglbl, "ALIGNMENT", "ACENTER");

    Ihandle* spc1 = IupSpace();
    IupSetAttribute(spc1, "SIZE", "1x10");

    Ihandle* progresslbl = IupFlatLabel(langGetC(self->ss, _S"ftl_progress"));
    IupSetAttribute(progresslbl, "FONT", "Helvetica, 12");
    IupSetAttribute(progresslbl, "FGCOLOR", "255 255 255");

    self->progressbar = IupGauge();
    IupSetAttribute(self->progressbar, "BACKCOLOR", panelbg);
    IupSetAttribute(self->progressbar, "FGCOLOR", "0 255 0");
    IupSetAttribute(self->progressbar, "FLAT", "YES");
    IupSetAttribute(self->progressbar, "FLATCOLOR", "255 255 255");
    IupSetAttribute(self->progressbar, "SIZE", "1x7");
    IupSetAttribute(self->progressbar, "EXPAND", "HORIZONTAL");
    IupSetAttribute(self->progressbar, "SHOWTEXT", "NO");
    Ihandle* progresshbox = IupHbox(self->progressbar, NULL);
    IupSetAttribute(progresshbox, "NCMARGIN", "10x0");

    self->loading = IupVbox(loadinglbl, spc1, progresslbl, progresshbox, IupFill(), NULL);
    IupSetAttribute(self->loading, "ALIGNMENT", "ACENTER");
    IupSetAttribute(self->loading, "NCGAP", "2");
    IupSetAttribute(self->loading, "NCMARGIN", "8x8");
}

static void makeAtmenu(GameInfoPanel* self)
{
    Ihandle* waitlbl = IupFlatLabel(langGetC(self->ss, _S"ftl_waitingforrun"));
    IupSetAttribute(waitlbl, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(waitlbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(waitlbl, "EXPAND", "YES");
    IupSetAttribute(waitlbl, "ALIGNMENT", "ACENTER");
    IupSetAttribute(waitlbl, "TEXTWRAP", "YES");

    Ihandle* nglbl = IupFlatLabel(langGetC(self->ss, _S"ftl_newgame"));
    IupSetAttribute(nglbl, "FONT", "Helvetica, 12");
    IupSetAttribute(nglbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(nglbl, "EXPAND", "YES");
    IupSetAttribute(nglbl, "ALIGNMENT", "ACENTER");
    IupSetAttribute(nglbl, "TEXTWRAP", "YES");

    Ihandle* gplbl = IupFlatLabel(langGetC(self->ss, _S"ftl_gopractice"));
    IupSetAttribute(gplbl, "FONT", "Helvetica, 12");
    IupSetAttribute(gplbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(gplbl, "EXPAND", "YES");
    IupSetAttribute(gplbl, "ALIGNMENT", "ACENTER");
    IupSetAttribute(gplbl, "TEXTWRAP", "YES");

    self->atmenu = IupVbox(waitlbl, nglbl, gplbl, NULL);
    IupSetAttribute(self->atmenu, "ALIGNMENT", "ACENTER");
    IupSetAttribute(self->atmenu, "CGAP", "4");
    IupSetAttribute(self->atmenu, "NCMARGIN", "8x8");
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool GameInfoPanel_make(_In_ GameInfoPanel* self)
{
    makeNotRunning(self);
    makeLoading(self);
    makeAtmenu(self);

    self->zbox = IupZbox(self->notrunning, self->loading, self->atmenu, NULL);
    IupSetAttribute(self->zbox, "CHILDSIZEALL", "NO");

    self->h = IupBackgroundBox(self->zbox);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    return parent_make();
}

static void gotoSubPanel(GameInfoPanel* self, Ihandle* subpanel)
{
    if (self->cursub != subpanel) {
        self->cursub = subpanel;
        IupSetAttributeHandle(self->zbox, "VALUE", subpanel);
    }
}

extern bool Panel_update(_In_ Panel* self);   // parent
#define parent_update() Panel_update((Panel*)(self))
bool GameInfoPanel_update(_In_ GameInfoPanel* self)
{
    GameInst* inst   = subspaceCurInst(self->ss);
    GameInstState st = GI_Init;
    bool running     = false;

    // play button disabling

    bool pbenabled = false;
    // update play button status
    if (inst) {
        st = ginstGetState(inst);
        if (st == GI_Init || st == GI_Exited)
            pbenabled = true;
    } else {
        pbenabled = true;
    }

    IupSetAttribute(self->playbtn, "ACTIVE", pbenabled ? "YES" : "NO");

    // sub-panel selection

    if (!inst) {
        gotoSubPanel(self, self->notrunning);
        goto out;
    }

    if (st == GI_Loading) {
        gotoSubPanel(self, self->loading);
        strFromFloat32(&self->loadpct, inst->loadPct);
        IupSetAttribute(self->progressbar, "VALUE", strC(self->loadpct));
        goto out;
    }

    if (st == GI_Menu) {
        gotoSubPanel(self, self->atmenu);
        goto out;
    }

out:
    objRelease(&inst);
    return true;
}

void GameInfoPanel_destroy(_In_ GameInfoPanel* self)
{
    // Autogen begins -----
    strDestroy(&self->loadpct);
    // Autogen ends -------
}

// Autogen begins -----
#include "gameinfopanel.auto.inc"
// Autogen ends -------
