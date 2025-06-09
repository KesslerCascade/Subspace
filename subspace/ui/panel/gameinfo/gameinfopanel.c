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
#include <cx/format.h>
#include <iupcontrols.h>
#include "gamemgr/gamemgr.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed GameInfoPanel* GameInfoPanel_create(SubspaceUI* ui)
{
    GameInfoPanel* self;
    self     = objInstCreate(GameInfoPanel);
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
    Ihandle* nrlabel1 = IupFlatLabel(langGetC(self->ss, "ftl_notrunning"));
    IupSetAttribute(nrlabel1, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(nrlabel1, "FGCOLOR", "255 255 255");
    IupSetAttribute(nrlabel1, "EXPAND", "HORIZONTAL");
    IupSetAttribute(nrlabel1, "ALIGNMENT", "ACENTER");

    Ihandle* hplabel1 = IupFlatLabel(langGetC(self->ss, "ftl_howtoplay1"));
    IupSetAttribute(hplabel1, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel1, "FGCOLOR", "255 255 255");
    Ihandle* hplabel2 = IupFlatLabel(langGetC(self->ss, "ftl_howtoplay2"));
    IupSetAttribute(hplabel2, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel2, "FGCOLOR", "255 255 255");
    Ihandle* hplabel3 = IupFlatLabel(langGetC(self->ss, "ftl_howtoplay3"));
    IupSetAttribute(hplabel3, "FONT", "Helvetica, 12");
    IupSetAttribute(hplabel3, "FGCOLOR", "255 255 255");
    self->playbtn = IupFlatButton(langGetC(self->ss, "play"));
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
    Ihandle* loadinglbl = IupFlatLabel(langGetC(self->ss, "ftl_loading"));
    IupSetAttribute(loadinglbl, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(loadinglbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(loadinglbl, "EXPAND", "HORIZONTAL");
    IupSetAttribute(loadinglbl, "ALIGNMENT", "ACENTER");

    Ihandle* spc1 = IupSpace();
    IupSetAttribute(spc1, "SIZE", "1x10");

    Ihandle* progresslbl = IupFlatLabel(langGetC(self->ss, "ftl_progress"));
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
    Ihandle* waitlbl = IupFlatLabel(langGetC(self->ss, "ftl_waitingforrun"));
    IupSetAttribute(waitlbl, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(waitlbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(waitlbl, "EXPAND", "YES");
    IupSetAttribute(waitlbl, "ALIGNMENT", "ACENTER");
    IupSetAttribute(waitlbl, "TEXTWRAP", "YES");

    Ihandle* nglbl = IupFlatLabel(langGetC(self->ss, "ftl_newgame"));
    IupSetAttribute(nglbl, "FONT", "Helvetica, 12");
    IupSetAttribute(nglbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(nglbl, "EXPAND", "YES");
    IupSetAttribute(nglbl, "ALIGNMENT", "ACENTER");
    IupSetAttribute(nglbl, "TEXTWRAP", "YES");

    Ihandle* gplbl = IupFlatLabel(langGetC(self->ss, "ftl_gopractice"));
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

static void makeTutorial(GameInfoPanel* self)
{
    Ihandle* ttitle = IupFlatLabel(langGetC(self->ss, "ftl_tutorial_title"));
    IupSetAttribute(ttitle, "FONT", "Helvetica, Bold 14");
    IupSetAttribute(ttitle, "FGCOLOR", "255 255 255");
    IupSetAttribute(ttitle, "EXPAND", "NO");
    IupSetAttribute(ttitle, "ALIGNMENT", "ACENTER");

    Ihandle* tlbl = IupFlatLabel(langGetC(self->ss, "ftl_tutorial_text"));
    IupSetAttribute(tlbl, "FONT", "Helvetica, 12");
    IupSetAttribute(tlbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(tlbl, "EXPAND", "YES");
    IupSetAttribute(tlbl, "ALIGNMENT", "ATOP");
    IupSetAttribute(tlbl, "TEXTWRAP", "YES");

    self->tutorial = IupVbox(ttitle, tlbl, NULL);
    IupSetAttribute(self->tutorial, "ALIGNMENT", "ACENTER");
    IupSetAttribute(self->tutorial, "CGAP", "8");
    IupSetAttribute(self->tutorial, "NCMARGIN", "8x8");
}

static void makeInfo(GameInfoPanel* self)
{
    self->shipname = IupFlatLabel("");
    IupSetAttribute(self->shipname, "EXPAND", "HORIZONTAL");
    self->shiptype = IupFlatLabel("");

    Ihandle* headerline1 = IupHbox(self->shipname, self->shiptype, NULL);
    IupSetAttribute(headerline1, "CMARGIN", "0x0");

    self->sector = IupFlatLabel("");
    IupSetAttribute(self->sector, "EXPAND", "HORIZONTAL");
    self->difficulty = IupFlatLabel("");

    Ihandle* headerline2 = IupHbox(self->sector, self->difficulty, NULL);
    IupSetAttribute(headerline2, "CMARGIN", "0x0");

    Ihandle* header   = IupVbox(headerline1, headerline2, NULL);
    Ihandle* headerbg = IupBackgroundBox(header);
    IupSetAttribute(headerbg, "BGCOLOR", "0 0 128");
    IupSetAttribute(headerbg, "FGCOLOR", "255 255 255");
    IupSetAttribute(headerbg, "FONT", "Helvetica, Bold 9");
    IupSetAttribute(headerbg, "CGAP", "2");
    IupSetAttribute(headerbg, "CMARGIN", "3x2");

    self->seed      = IupFlatLabel("");
    Ihandle* footer = IupHbox(IupFill(), self->seed, NULL);
    IupSetAttribute(footer, "CMARGIN", "3x2");
    Ihandle* footerbg = IupBackgroundBox(footer);
    IupSetAttribute(footerbg, "BGCOLOR", "0 0 128");
    IupSetAttribute(footerbg, "FGCOLOR", "255 255 255");
    IupSetAttribute(footerbg, "FONT", "Helvetica, Bold 8");

    self->statsbox = IupMatrix("");
    IupSetAttribute(self->statsbox, "CURSOR", "ARROW");
    IupSetAttribute(self->statsbox, "FLAT", "YES");
    IupSetAttribute(self->statsbox, "READONLY", "YES");
    IupSetAttribute(self->statsbox, "HIDEFOCUS", "YES");
    IupSetAttribute(self->statsbox, "NUMCOL", "1");
    IupSetAttribute(self->statsbox, "NUMLIN", "6");
    IupSetAttribute(self->statsbox, "BGCOLOR", panelbg);
    IupSetAttribute(self->statsbox, "BGCOLOR*:0", "56 56 56");
    IupSetAttribute(self->statsbox, "FGCOLOR", "255 255 255");
    IupSetAttribute(self->statsbox, "EXPAND", "YES");
    IupSetAttribute(self->statsbox, "FLATSCROLLBAR", "VERTICAL");
    IupSetAttribute(self->statsbox, "FRAMECOLOR", "48 48 48");
    IupSetAttribute(self->statsbox, "ALIGNMENT0", "ARIGHT");
    IupSetAttribute(self->statsbox, "ALIGNMENT1", "ALEFT");
    IupSetAttribute(self->statsbox, "1:0", langGetC(self->ss, "runinfo_started"));
    IupSetAttribute(self->statsbox, "2:0", langGetC(self->ss, "runinfo_scrap"));
    IupSetAttribute(self->statsbox, "3:0", langGetC(self->ss, "runinfo_beacons"));
    IupSetAttribute(self->statsbox, "4:0", langGetC(self->ss, "runinfo_ships"));
    IupSetAttribute(self->statsbox, "5:0", langGetC(self->ss, "runinfo_crew"));
    IupSetAttribute(self->statsbox, "6:0", langGetC(self->ss, "runinfo_score"));

    self->info = IupVbox(headerbg, self->statsbox, footerbg, NULL);
}

static int panelResize(Ihandle* ih, int width, int height)
{
    GameInfoPanel* self = iupGetParentObj(GameInfoPanel, ih);
    if (self) {
        int titlewidth = IupGetInt(self->statsbox, "RASTERWIDTH0");
        IupSetInt(self->statsbox, "RASTERWIDTH1", width - titlewidth);
    }

    return IUP_DEFAULT;
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool GameInfoPanel_make(_In_ GameInfoPanel* self)
{
    makeNotRunning(self);
    makeLoading(self);
    makeAtmenu(self);
    makeTutorial(self);
    makeInfo(self);

    self->zbox = IupZbox(self->notrunning,
                         self->loading,
                         self->atmenu,
                         self->tutorial,
                         self->info,
                         NULL);
    IupSetAttribute(self->zbox, "CHILDSIZEALL", "NO");

    self->h = IupBackgroundBox(self->zbox);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);
    iupSetObj(self->h, ObjNone, self, self->ui);
    IupSetCallback(self->h, "RESIZE_CB", (Icallback)panelResize);

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

    if (st == GI_Tutorial) {
        gotoSubPanel(self, self->tutorial);
        goto out;
    }

    if (st == GI_Run) {
        RunInfo* run = subspaceCurRun(self->ss);
        if (run) {
            gotoSubPanel(self, self->info);

            withReadLock (&run->lock) {
                string temp = 0, temp2 = 0;

                IupSetStrAttribute(self->shipname, "TITLE", strC(run->shipName));
                IupSetStrAttribute(self->shiptype,
                                   "TITLE",
                                   strC(run->shipType));   // TODO: Translate
                if (saSize(run->sectors) > 0) {
                    SectorInfo* last = run->sectors.a[saSize(run->sectors) - 1];

                    spointFormat(&temp2, last->sectorpoint);
                    strFormat(&temp,
                              langGet(self->ss, _S"runinfo_sector_format"),
                              stvar(strref, temp2),
                              stvar(strref, last->type));   // TODO: Translate
                    IupSetStrAttribute(self->sector, "TITLE", strC(temp));
                } else {
                    IupSetAttribute(self->sector,
                                    "TITLE",
                                    langGetC(self->ss, "runinfo_sector_unknown"));
                }

                string diffstr = _S"difficulty_unknown";
                if (run->difficulty == 2)
                    diffstr = _S"difficulty_hard";
                else if (run->difficulty == 1)
                    diffstr = _S"difficulty_normal";
                else if (run->difficulty == 0)
                    diffstr = _S"difficulty_easy";

                IupSetStrAttribute(self->difficulty, "TITLE", strC(langGet(self->ss, diffstr)));

                strFormat(&temp, langGet(self->ss, _S"runinfo_seed_format"), stvar(int32, run->seed));
                IupSetStrAttribute(self->seed, "TITLE", strC(temp));

                // update stats
                int64 started = toLocalTime(run->startTime);
                TimeParts p;
                timeDecompose(&p, started);

                strFormat(&temp2, _S"weekday_short${int}", stvar(int32, p.wday));
                strFormat(&temp,
                          _S"${string} ${int}-${0int(2)}-${0int(2)} ${int}:${0int(2)}:${0int(2)}",
                          stvar(strref, langGet(self->ss, temp2)),
                          stvar(int32, p.year),
                          stvar(int32, p.month),
                          stvar(int32, p.day),
                          stvar(int32, p.hour),
                          stvar(int32, p.minute),
                          stvar(int32, p.second));
                IupSetStrAttribute(self->statsbox, "1:1", strC(temp));

                // TODO: Add actual if known
                strFromInt32(&temp, run->scrapCollected, 10);
                IupSetStrAttribute(self->statsbox, "2:1", strC(temp));

                strFromInt32(&temp, run->beaconsExplored, 10);
                IupSetStrAttribute(self->statsbox, "3:1", strC(temp));

                strFromInt32(&temp, run->shipsDefeated, 10);
                IupSetStrAttribute(self->statsbox, "4:1", strC(temp));

                strFromInt32(&temp, run->crewHired, 10);
                IupSetStrAttribute(self->statsbox, "5:1", strC(temp));

                strFromInt32(&temp, runinfoScore(run), 10);
                IupSetStrAttribute(self->statsbox, "6:1", strC(temp));

                IupSetAttribute(self->statsbox, "REDRAW", "ALL");

                IupRefresh(self->info);
            }
            objRelease(&run);
        }
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
