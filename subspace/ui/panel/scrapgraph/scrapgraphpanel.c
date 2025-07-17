// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "scrapgraphpanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <iup_plot.h>
#include "control/controlserver.h"
#include "run/logrelay.h"
#include "run/runinfo.h"
#include "run/scraptracker.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed ScrapGraphPanel* ScrapGraphPanel_create(SubspaceUI* ui)
{
    ScrapGraphPanel* self;
    self = objInstCreate(ScrapGraphPanel);

    self->ss = ui->ss;
    self->ui = ui;
    strDup(&self->title, langGet(self->ss, _S"panel_scrapgraph"));
    self->name = _S"scrapgraph";

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool ScrapGraphPanel_init(_In_ ScrapGraphPanel* self)
{
    // Autogen begins -----
    saInit(&self->sectoridx, int64, 1, SA_Sorted);
    return true;
    // Autogen ends -------
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool ScrapGraphPanel_make(_In_ ScrapGraphPanel* self)
{
    self->plot = IupPlot();
    IupPlotBegin(self->plot, 1);
    self->ds = IupPlotEnd(self->plot);
    IupSetAttribute(self->plot, "FGCOLOR", "255 255 255");
    IupSetAttribute(self->plot, "AXS_YAUTOMIN", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKAUTO", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKMINORDIVISION", "1");
    IupSetAttribute(self->plot, "AXS_YTIPFORMAT", "%.0f");
    IupSetAttribute(self->plot, "TIPFORMAT", "%.0s%.0s%s");

    self->h = IupBackgroundBox(self->plot);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);
    iupSetObj(self->h, self, ObjNone, self->ui);

    // get UI updates from the scrap tracker
    ssuiListen(self->ui, self, _S"Scrap_Update");
    ssuiListen(self->ui, self, _S"Scrap_Reset");
    ssuiListen(self->ui, self, _S"Scrap_Refresh");
    // register to receive new sector events
    logrelaySubscribeUI(self->ss->runlog, self, _S"Sector");

    return parent_make();
}

void ScrapGraphPanel_clear(_In_ ScrapGraphPanel* self)
{
    string temp = 0;

    saClear(&self->sectoridx);
    IupSetAttribute(self->plot, "CLEAR", "1");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        saPush(&self->sectoridx, int64, SPOINT(i, 0));
        strFromInt32(&temp, i, 10);
        IupPlotAddStr(self->plot, strC(temp), 0);
    }
    self->ds = IupPlotEnd(self->plot);
    IupSetAttribute(self->plot, "DS_NAME", "Scrap");
    IupSetAttribute(self->plot, "DS_COLOR", "200 40 40");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARSPACING", "30");

    strDestroy(&temp);
}

void ScrapGraphPanel_logNotify(_In_ ScrapGraphPanel* self, LogEnt* ent, bool replay)
{
    if (strEq(ent->id, _S"Sector"))
        scrapgraphpanelHandleUpdate(self, ent->sectorpoint, true);
}

void ScrapGraphPanel_logReset(_In_ ScrapGraphPanel* self)
{
    return;
}

void ScrapGraphPanel_logReplayComplete(_In_ ScrapGraphPanel* self)
{
    return;
}

void ScrapGraphPanel_uiNotify(_In_ ScrapGraphPanel* self, _In_opt_ strref event, stvlist* params)
{
    if (strEq(event, _S"Scrap_Update")) {
        int64 savepoint = 0, sectorpoint = 0;
        bool replay = false;
        stvlNext(params, int64, &savepoint);
        stvlNext(params, int64, &sectorpoint);
        stvlNext(params, bool, &replay);

        scrapgraphpanelHandleUpdate(self, sectorpoint, !replay);
    } else if (strEq(event, _S"Scrap_Reset")) {
        scrapgraphpanelClear(self);
    } else if (strEq(event, _S"Scrap_Refresh")) {
        IupSetAttribute(self->plot, "REDRAW", NULL);
    }
}

// This must be called from the UI thread!
void ScrapGraphPanel_handleUpdate(_In_ ScrapGraphPanel* self, int64 sectorpoint, bool redraw)
{
    string temp = 0;

    RunInfo* run = subspaceRun(self->ss);
    if (!run)
        return;

    spointFormat(&temp, sectorpoint);

    SectorInfo* sec = runinfoGetSector(run, sectorpoint);
    if (!sec)
        goto out;

    // see if there's a translation for the sector name abbreviation
    string skey = 0;
    strConcat(&skey, _S"sector_shortname_", sec->type);
    strref sname = langGetD(self->ss, skey, _S"");
    if (!strEmpty(sname)) {
        strDup(&temp, sname);
    }
    strDestroy(&skey);

    ScrapTracker* tracker = runinfoGetScrap(run);
    if (tracker) {
        ScrapTotals total = { 0 };
        scraptrackerGetSector(tracker, sectorpoint, &total);

        // get the index of the plot sample for the sector's bar
        int32 idx = saFind(self->sectoridx, int64, sectorpoint);
        if (idx >= 0) {
            IupPlotSetSampleStr(self->plot, self->ds, idx, strC(temp), total.rewards);
        } else {
            // new sector we haven't seen before, probably crystal
            idx = saPush(&self->sectoridx, int64, sectorpoint);
            IupPlotInsertStr(self->plot, self->ds, idx, strC(temp), total.rewards);
        }
        objRelease(&tracker);
    }

    if (redraw)
        IupSetAttribute(self->plot, "REDRAW", NULL);

out:
    strDestroy(&temp);
    objRelease(&sec);
    objRelease(&run);
}

void ScrapGraphPanel_destroy(_In_ ScrapGraphPanel* self)
{
    // Autogen begins -----
    saDestroy(&self->sectoridx);
    // Autogen ends -------
}

// Autogen begins -----
#include "scrapgraphpanel.auto.inc"
// Autogen ends -------
