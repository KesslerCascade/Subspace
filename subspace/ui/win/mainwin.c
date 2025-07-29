// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "mainwin.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "feature/featureregistry.h"
#include "feature/runtracker/runtracker.h"
#include "gamemgr/gamemgr.h"
#include "run/runinfo.h"
#include "ui/panel/gameinfo/gameinfopanel.h"
#include "ui/panel/hullgraph/hullgraphpanel.h"
#include "ui/panel/runsummary/runsummarypanel.h"
#include "ui/panel/scrapgraph/scrapgraphpanel.h"
#include "ui/panel/sectordetail/sectordetailpanel.h"
#include "ui/panel/welcome/welcomepanel.h"
#include "ui/subspaceui.h"
#include "ui/util/iuploadimage.h"
#include "ui/util/iupsetobj.h"
#include "quitconfirm.h"
#include "settingswin.h"

#define REGISTERPANEL(mprefix)         \
    p = Panel(mprefix##Create(ui));    \
    saPush(&titles, string, p->title); \
    htInsertC(&tpanels, strref, p->title, object, &p)

void MainWin_makeMenu(_In_ MainWin* self);
static void MainWin_registerPanels(MainWin* self, SubspaceUI* ui)
{
    Panel* p;

    sa_string titles;
    saInit(&titles, string, 8, SA_Sorted);
    hashtable tpanels;   // temp hashtable
    htInit(&tpanels, string, object, 8);

    REGISTERPANEL(gameinfopanel);
    REGISTERPANEL(hullgraphpanel);
    REGISTERPANEL(runsummarypanel);
    REGISTERPANEL(scrapgraphpanel);
    REGISTERPANEL(sectordetailpanel);

    // insert them into the real hashtable in sorted order, for convenience later
    foreach (sarray, idx, string, title, titles) {
        htelem e = htFind(tpanels, string, title, none, NULL);
        p        = (Panel*)hteVal(tpanels, object, e);
        htInsert(&self->panels, strref, p->name, object, p);
    }

    htDestroy(&tpanels);
    saDestroy(&titles);

    self->welcomepanel = Panel(welcomepanelCreate(ui));
}

_objfactory_guaranteed MainWin* MainWin_create(SubspaceUI* ui)
{
    MainWin* self;
    self = objInstCreate(MainWin);

    self->ui = ui;
    self->ss = ui->ss;

    objInstInit(self);
    MainWin_registerPanels(self, self->ui);

    return self;
}

_objinit_guaranteed bool MainWin_init(_In_ MainWin* self)
{
    // Autogen begins -----
    htInit(&self->panels, string, object, 16);
    return true;
    // Autogen ends -------
}

static int settingsbtn_action(Ihandle* ih)
{
    SubspaceUI* ui = iupGetUI(ih);
    if (!ui)
        return IUP_IGNORE;

    settingswinShow(ui->settingsw);
    return IUP_DEFAULT;
}

static int playbtn_action(Ihandle* ih)
{
    Subspace* ss = iupGetSubspace(ih);
    if (!ss)
        return IUP_IGNORE;

    gmgrPlay(ss->gmgr, NULL);
    return IUP_DEFAULT;
}

static int runtrackerbtn_action(Ihandle* ih)
{
    Subspace* ss = iupGetSubspace(ih);
    MainWin* win = iupGetParentObj(MainWin, ih);
    if (!ss || !win)
        return IUP_IGNORE;

    RunTracker* rt = fregGet(RunTracker, ss->freg);

    // same rules as feature enable/disable apply here; can't change it while the game is running
    if (!featureIsLocked(rt)) {
        runtrackerPause(rt, !runtrackerIsPaused(rt));
        mainwinUpdate(win);
    }

    return IUP_DEFAULT;
}

static int menubtn_action(Ihandle* ih)
{
    MainWin* win = iupGetParentObj(MainWin, ih);
    if (win) {
        int mx, my;
        IupGetIntInt(NULL, "CURSORPOS", &mx, &my);
        mainwinShowMenu(win, mx, my);
    }
    return IUP_DEFAULT;
}

bool MainWin_make(_In_ MainWin* self)
{
    self->timer = IupTimer();
    iupSetObj(self->timer, ObjNone, self, self->ui);
    IupSetCallback(self->timer, "ACTION_CB", MainWin_onTimer);
    IupSetAttribute(self->timer, "TIME", "10000");
    IupSetAttribute(self->timer, "RUN", "YES");

    foreach (hashtable, hti, self->panels) {
        Panel* p = objDynCast(Panel, htiVal(object, hti));
        if (p)
            panelMake(p);
    }
    panelMake(self->welcomepanel);

    // wait on these three
    iupLoadImageWait(self->ss,
                     _S"IMAGE_RUNTRACKER_SIDEBAR",
                     _S"svg",
                     _S"subspace:/runtrackerbar.svg");
    iupLoadImageWait(self->ss, _S"IMAGE_HAMBURGER", _S"svg", _S"subspace:/hamburger.svg");
    iupLoadImageWait(self->ss, _S"IMAGE_SETTINGS", _S"svg", _S"subspace:/settings.svg");
    iupLoadImageWait(self->ss, _S"IMAGE_PLAY", _S"svg", _S"subspace:/play.svg");

    self->menubtn = IupFlatButton(NULL);
    IupSetAttribute(self->menubtn, "IMAGE", "IMAGE_HAMBURGER");
    IupSetAttribute(self->menubtn, "IMAGEHIGHLIGHT", "IMAGE_HAMBURGER_HOVER");
    IupSetAttribute(self->menubtn, "HLCOLOR", NULL);
    IupSetAttribute(self->menubtn, "PSCOLOR", NULL);
    IupSetAttribute(self->menubtn, "BORDERWIDTH", "0");
    IupSetAttribute(self->menubtn, "TIP", langGetC(self->ss, "hamburger_tip"));
    iupSetObj(self->menubtn, ObjNone, self, self->ui);
    IupSetCallback(self->menubtn, "FLAT_ACTION", menubtn_action);

    Ihandle* settings = IupFlatButton(NULL);
    IupSetAttribute(settings, "IMAGE", "IMAGE_SETTINGS");
    IupSetAttribute(settings, "IMAGEHIGHLIGHT", "IMAGE_SETTINGS_HOVER");
    IupSetAttribute(settings, "HLCOLOR", NULL);
    IupSetAttribute(settings, "PSCOLOR", NULL);
    IupSetAttribute(settings, "BORDERWIDTH", "0");
    IupSetAttribute(settings, "TIP", langGetC(self->ss, "settings_tip"));
    iupSetObj(settings, ObjNone, self, self->ui);
    IupSetCallback(settings, "FLAT_ACTION", settingsbtn_action);

    self->playbtn = IupFlatButton(NULL);
    IupSetAttribute(self->playbtn, "IMAGE", "IMAGE_PLAY");
    IupSetAttribute(self->playbtn, "IMAGEHIGHLIGHT", "IMAGE_PLAY_HOVER");
    IupSetAttribute(self->playbtn, "IMAGEINACTIVE", "IMAGE_PLAY_DISABLED");
    IupSetAttribute(self->playbtn, "HLCOLOR", NULL);
    IupSetAttribute(self->playbtn, "PSCOLOR", NULL);
    IupSetAttribute(self->playbtn, "BORDERWIDTH", "0");
    IupSetAttribute(self->playbtn, "TIP", langGetC(self->ss, "play_tip"));
    iupSetObj(self->playbtn, ObjNone, self, self->ui);
    IupSetCallback(self->playbtn, "FLAT_ACTION", playbtn_action);

    self->runtrackerbtn = IupFlatButton(NULL);
    IupSetAttribute(self->runtrackerbtn, "IMAGE", "IMAGE_RUNTRACKER_SIDEBAR");
    IupSetAttribute(self->runtrackerbtn, "IMAGEHIGHLIGHT", "IMAGE_RUNTRACKER_HOVER");
    IupSetAttribute(self->runtrackerbtn, "HLCOLOR", NULL);
    IupSetAttribute(self->runtrackerbtn, "PSCOLOR", NULL);
    IupSetAttribute(self->runtrackerbtn, "BORDERWIDTH", "0");
    IupSetStrAttribute(self->runtrackerbtn, "TIP", langGetC(self->ss, "runtracker_ready"));
    IupSetAttribute(self->runtrackerbtn, "VISIBLE", "NO");
    iupSetObj(self->runtrackerbtn, ObjNone, self, self->ui);
    IupSetCallback(self->runtrackerbtn, "FLAT_ACTION", runtrackerbtn_action);

    self->sidebar = IupVbox(self->menubtn, self->playbtn, settings, self->runtrackerbtn, NULL);
    IupSetAttribute(self->sidebar, "CGAP", "2");
    IupSetAttribute(self->sidebar, "NCMARGIN", "2x2");
    iupSetObj(self->sidebar, ObjNone, self, self->ui);

    self->barsep = IupFlatSeparator();
    self->zbox   = IupZbox(self->welcomepanel->h, NULL);
    mainwinLoadLayout(self);
    IupAppend(self->zbox, self->root);

    string tmp = 0;
    if (ssdStringOut(self->ss->settings, _S"ftl/exe", &tmp)) {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->root);
    } else {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->welcomepanel->h);
    }

    self->win = IupDialog(IupHbox(self->sidebar, self->barsep, self->zbox, NULL));
    IupSetAttribute(self->win, "MINSIZE", "500x300");   // pixels, not the same units as SIZE
    self->width  = ssdVal(int32, self->ss->settings, _S"ui/size/width", MAINWIN_DEFAULT_WIDTH);
    self->height = ssdVal(int32, self->ss->settings, _S"ui/size/height", MAINWIN_DEFAULT_HEIGHT);
    strFormat(&tmp, _S"${int}x${int}", stvar(int32, self->width), stvar(int32, self->height));
    IupSetAttribute(self->win, "SIZE", strC(tmp));
    IupSetAttribute(self->win, "SHRINK", "Yes");
    IupSetAttribute(self->win, "TITLE", langGetC(self->ss, "subspace_title"));
    IupSetAttribute(self->win, "ICON", "SUBSPACE_ICON");
    iupSetObj(self->win, self, ObjNone, self->ui);

    IupSetHandle("SUBSPACE_MAINWIN", self->win);

    IupSetCallback(self->win, "CLOSE_CB", (Icallback)MainWin_onClose);
    IupSetCallback(self->win, "RESIZE_CB", (Icallback)MainWin_onResize);

    MainWin_makeMenu(self);

    strDestroy(&tmp);
    return true;
}

void MainWin_show(_In_ MainWin* self)
{
    IupShowXY(self->win, IUP_CENTER, IUP_CENTER);
    mainwinUpdateAll(self);

    // Background load the images that aren't needed right away (must do this AFTER map for refresh
    // to work)
    iupLoadImage(self->ss, _S"IMAGE_HAMBURGER_HOVER", _S"svg", _S"subspace:/hamburger-hover.svg", NULL);
    iupLoadImage(self->ss, _S"IMAGE_SETTINGS_HOVER", _S"svg", _S"subspace:/settings-hover.svg", NULL);
    iupLoadImage(self->ss, _S"IMAGE_PLAY_HOVER", _S"svg", _S"subspace:/play-hover.svg", NULL);
    iupLoadImage(self->ss,
                 _S"IMAGE_RUNTRACKER_HOVER",
                 _S"svg",
                 _S"subspace:/runtrackerbar-hover.svg",
                 NULL);
    iupLoadImage(self->ss,
                 _S"IMAGE_RUNTRACKER_RECORDING",
                 _S"svg",
                 _S"subspace:/runtrackerbar-recording.svg",
                 NULL);
    iupLoadImage(self->ss,
                 _S"IMAGE_RUNTRACKER_PAUSED",
                 _S"svg",
                 _S"subspace:/runtrackerbar-paused.svg",
                 NULL);
    iupLoadImage(self->ss,
                 _S"IMAGE_RUNTRACKER_PAUSED_HOVER",
                 _S"svg",
                 _S"subspace:/runtrackerbar-paused-hover.svg",
                 NULL);
    iupLoadImage(self->ss, _S"IMAGE_PLAY_HOVER", _S"svg", _S"subspace:/play-hover.svg", NULL);
    iupLoadImage(self->ss,
                 _S"IMAGE_PLAY_DISABLED",
                 _S"svg",
                 _S"subspace:/play-disabled.svg",
                 self->playbtn);
}

void MainWin_update(_In_ MainWin* self)
{
    string tmp   = 0;
    bool haveexe = false, pbenabled = false;
    ;
    if (ssdStringOut(self->ss->settings, _S"ftl/exe", &tmp)) {
        haveexe = true;
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->root);
    } else {
        IupSetAttribute(self->zbox, "VALUE_HANDLE", (char*)self->welcomepanel->h);
    }
    strDestroy(&tmp);

    // update play button state
    if (haveexe) {
        GameInst* inst = subspaceGame(self->ss);
        if (inst) {
            GameInstState st = ginstGetState(inst);
            if (st == GI_Init || st == GI_Exited)
                pbenabled = true;
            objRelease(&inst);
        } else {
            pbenabled = true;
        }
    }

    // update run tracker button state
    RunTracker* rt = fregGet(RunTracker, self->ss->freg);
    if (!rt || !featureIsEnabled(rt)) {
        IupSetAttribute(self->runtrackerbtn, "VISIBLE", "NO");
    } else {
        RunInfo* run = subspaceRun(self->ss);
        if (run && runinfoIsRecording(run)) {
            IupSetAttribute(self->runtrackerbtn, "IMAGE", "IMAGE_RUNTRACKER_RECORDING");
            IupSetAttribute(self->runtrackerbtn, "IMAGEHIGHLIGHT", "IMAGE_RUNTRACKER_RECORDING");
            IupSetStrAttribute(self->runtrackerbtn,
                               "TIP",
                               langGetC(self->ss, "runtracker_recording"));
        } else if (runtrackerIsPaused(rt)) {
            IupSetAttribute(self->runtrackerbtn, "IMAGE", "IMAGE_RUNTRACKER_PAUSED");
            // only give hover feedback if feature is unlocked and can be toggled
            if (!featureIsLocked(rt))
                IupSetAttribute(self->runtrackerbtn,
                                "IMAGEHIGHLIGHT",
                                "IMAGE_RUNTRACKER_PAUSED_HOVER");
            else
                IupSetAttribute(self->runtrackerbtn, "IMAGEHIGHLIGHT", "IMAGE_RUNTRACKER_PAUSED");
            IupSetStrAttribute(self->runtrackerbtn, "TIP", langGetC(self->ss, "runtracker_paused"));
        } else {
            IupSetAttribute(self->runtrackerbtn, "IMAGE", "IMAGE_RUNTRACKER_SIDEBAR");
            IupSetAttribute(self->runtrackerbtn, "IMAGEHIGHLIGHT", "IMAGE_RUNTRACKER_HOVER");
            IupSetStrAttribute(self->runtrackerbtn, "TIP", langGetC(self->ss, "runtracker_ready"));
        }
        IupSetAttribute(self->runtrackerbtn, "VISIBLE", "YES");
        objRelease(&run);
    }
    IupUpdate(self->runtrackerbtn);

    IupSetAttribute(self->playbtn, "ACTIVE", pbenabled ? "YES" : "NO");

    return;
}

void MainWin_updateAll(_In_ MainWin* self)
{
    MainWin_update(self);

    // update all the panels
    foreach (hashtable, hti, self->panels) {
        Panel* panel = (Panel*)htiVal(object, hti);
        panelUpdate(panel);
    }
}

bool MainWin_updatePanel(_In_ MainWin* self, _In_opt_ strref name)
{
    Panel* panel;
    if (htFind(self->panels, strref, name, object, &panel)) {
        panelUpdate(panel);
        objRelease(&panel);
        return true;
    }

    return false;
}

void MainWin_finish(_In_ MainWin* self)
{
    if (self->win)
        IupDestroy(self->win);
    if (self->timer)
        IupDestroy(self->timer);
    if (self->menu)
        IupDestroy(self->menu);
    if (self->layoutmenu)
        IupDestroy(self->layoutmenu);
    self->win        = NULL;
    self->timer      = NULL;
    self->menu       = NULL;
    self->layoutmenu = NULL;
}

void MainWin_destroy(_In_ MainWin* self)
{
    MainWin_finish(self);
    // Autogen begins -----
    htDestroy(&self->panels);
    objRelease(&self->welcomepanel);
    // Autogen ends -------
}

static void checkLayout(MainWin* self)
{
    if (self->layoutDirty) {
        self->layoutDirty = false;
        mainwinSaveLayout(self);
    }
}

int MainWin_onClose(Ihandle* ih)
{
    bool quit     = true;
    MainWin* self = iupGetObj(MainWin, ih);
    checkLayout(self);

    // check if a recorded run is in progress
    RunInfo* run = subspaceRun(self->ss);
    if (run && runinfoIsRecording(run))
        quit = confirmQuit(self->ss);
    objRelease(&run);

    if (quit) {
        self->ss->exit = true;
        eventSignal(&self->ss->notify);
    }

    return quit ? IUP_DEFAULT : IUP_IGNORE;
}

int MainWin_onResize(Ihandle* ih, int width, int height)
{
    MainWin* self = iupGetObj(MainWin, ih);
    if (IupGetInt(ih, "MINIMIZED") == 0)
        mainwinRecalcSize(self);

    if (IupGetInt(ih, "MAXIMIZED") > 0 || IupGetInt(ih, "MINIMIZED") > 0)
        return IUP_DEFAULT;   // don't save maximized "size"

    int w, h;
    if (IupGetIntInt(ih, "SIZE", &w, &h) == 2 && (w != self->width || h != self->height)) {
        self->width  = w;
        self->height = h;
        mainwinSetLayoutDirty(self);
    }

    return IUP_DEFAULT;
}

void MainWin_recalcSize(_In_ MainWin* self)
{
    int w, h, sw, sh, bw, bh;
    IupGetIntInt(self->win, "CLIENTSIZE", &w, &h);
    IupGetIntInt(self->sidebar, "RASTERSIZE", &sw, &sh);
    IupGetIntInt(self->barsep, "RASTERSIZE", &bw, &bh);
    IupSetStrf(self->root, "RASTERSIZE", "%dx%d", w - sw - bw, h);
    IupSetStrf(self->root, "MAXSIZE", "%dx%d", w - sw - bw, h);
}

int MainWin_onTimer(Ihandle* ih)
{
    MainWin* self = iupGetParentObj(MainWin, ih);
    if (self)
        checkLayout(self);
    return IUP_DEFAULT;
}

void MainWin_setLayoutDirty(_In_ MainWin* self)
{
    self->layoutDirty = true;
}

// Autogen begins -----
void MainWin_makeMenu(_In_ MainWin* self);
void MainWin_showMenu(_In_ MainWin* self, int mx, int my);
void MainWin_showLayoutMenu(_In_ MainWin* self, Ihandle* tabparent, Ihandle* ih, int mx, int my);
void MainWin_loadLayout(_In_ MainWin* self);
void MainWin_saveLayout(_In_ MainWin* self);
bool MainWin_isPanelInLayout(_In_ MainWin* self, _In_opt_ strref name);
Ihandle* MainWin_createPlaceholder(_In_ MainWin* self);
Ihandle* MainWin_createTabs(_In_ MainWin* self);
Ihandle* MainWin_createSplit(_In_ MainWin* self, bool vertical);
void MainWin_replaceSplitChild(_In_ MainWin* self, Ihandle* split, Ihandle* oh, Ihandle* nh, bool destroy);
void MainWin_addTab(_In_ MainWin* self, Ihandle* attachto, Ihandle* addbefore, _In_opt_ strref name);
void MainWin_removeTab(_In_ MainWin* self, Ihandle* tabparent, Ihandle* toremove);
void MainWin_addSplit(_In_ MainWin* self, Ihandle* at, bool vertical);
void MainWin_removePlaceholder(_In_ MainWin* self, Ihandle* ph);
#include "mainwin.auto.inc"
// Autogen ends -------
