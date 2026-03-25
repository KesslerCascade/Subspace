// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ui/panel/hullgraph/hullgraphpanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include <iup_plot.h>
#include "control/controlserver.h"
#include "run/hulltracker.h"
#include "run/runinfo.h"
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

_objfactory_guaranteed HullGraphPanel* HullGraphPanel_create(SubspaceUI* ui)
{
    HullGraphPanel* self;
    self = objInstCreate(HullGraphPanel);

    self->ss = ui->ss;
    self->ui = ui;
    strDup(&self->title, langGet(self->ss, _S"panel_hullgraph"));
    self->name = _S"hullgraph";

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool HullGraphPanel_init(_In_ HullGraphPanel* self)
{
    // Autogen begins -----
    saInit(&self->sectoridx, int64, 1, SA_Sorted);
    return true;
    // Autogen ends -------
}

void HullGraphPanel_onHullUpdate(_In_ HullGraphPanel* self, _In_opt_ strref event, stvlist* params)
{
    int64 savepoint = 0, sectorpoint = 0;
    bool replay = false;

    stvlNext(params, int64, &savepoint);
    stvlNext(params, int64, &sectorpoint);
    stvlNext(params, bool, &replay);

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

    HullTracker* tracker = runinfoGetHull(run);
    if (tracker) {
        HullTotals total = { 0 };
        hulltrackerGetSector(tracker, sectorpoint, &total);

        // get the index of the plot sample for the sector's bar
        int32 idx = saFind(self->sectoridx, int64, sectorpoint);
        if (idx >= 0) {
            IupPlotSetSampleStr(self->plot, self->repairds, idx, strC(temp), total.repair);
            IupPlotSetSampleStr(self->plot, self->ds[0], idx, "", total.projectile);
            IupPlotSetSampleStr(self->plot, self->ds[1], idx, "", total.missile);
            IupPlotSetSampleStr(self->plot, self->ds[2], idx, "", total.beam);
            IupPlotSetSampleStr(self->plot, self->ds[3], idx, "", total.asteroid);
            IupPlotSetSampleStr(self->plot, self->ds[4], idx, "", total.solarflare);
            IupPlotSetSampleStr(self->plot, self->ds[5], idx, "", total.asb);
            IupPlotSetSampleStr(self->plot, self->ds[6], idx, "", total.systemdestroyed);
            IupPlotSetSampleStr(self->plot, self->ds[7], idx, "", total.event);
            IupPlotSetSampleStr(self->plot, self->ds[8], idx, "", total.other);
        } else {
            // new sector we haven't seen before, probably crystal
            idx = saPush(&self->sectoridx, int64, sectorpoint);
            IupPlotInsertStr(self->plot, self->repairds, idx, strC(temp), total.repair);
            IupPlotInsertStr(self->plot, self->ds[0], idx, "", total.projectile);
            IupPlotInsertStr(self->plot, self->ds[1], idx, "", total.missile);
            IupPlotInsertStr(self->plot, self->ds[2], idx, "", total.beam);
            IupPlotInsertStr(self->plot, self->ds[3], idx, "", total.asteroid);
            IupPlotInsertStr(self->plot, self->ds[4], idx, "", total.solarflare);
            IupPlotInsertStr(self->plot, self->ds[5], idx, "", total.asb);
            IupPlotInsertStr(self->plot, self->ds[6], idx, "", total.systemdestroyed);
            IupPlotInsertStr(self->plot, self->ds[7], idx, "", total.event);
            IupPlotInsertStr(self->plot, self->ds[8], idx, "", total.other);
        }
        objRelease(&tracker);
    }

    if (!replay)
        IupSetAttribute(self->plot, "REDRAW", NULL);

out:
    strDestroy(&temp);
    objRelease(&sec);
    objRelease(&run);
}

void HullGraphPanel_onHullReset(_In_ HullGraphPanel* self, _In_opt_ strref event, stvlist* params)
{
    hullgraphpanelClear(self);
}

void HullGraphPanel_onHullRefresh(_In_ HullGraphPanel* self, _In_opt_ strref event, stvlist* params)
{
    IupSetAttribute(self->plot, "REDRAW", NULL);
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool HullGraphPanel_make(_In_ HullGraphPanel* self)
{
    self->plot = IupPlot();

    IupSetAttribute(self->plot, "FGCOLOR", "255 255 255");
    IupSetAttribute(self->plot, "AXS_YAUTOMIN", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKAUTO", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKMINORDIVISION", "1");
    IupSetAttribute(self->plot, "AXS_YTIPFORMAT", "%.0f");
    IupSetAttribute(self->plot, "TIPFORMAT", "%0s: %.0s%s");

    IupSetAttribute(self->plot, "LEGEND", "YES");
    IupSetAttribute(self->plot, "LEGENDPOS", "RIGHTMARGIN");

    self->h = IupBackgroundBox(self->plot);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);
    iupSetObj(self->h, self, ObjNone, self->ui);

    // get UI updates from the hull tracker
    ssuiListen(self->ui, self, HullGraphPanel_onHullUpdate, _S"Hull_Update");
    ssuiListen(self->ui, self, HullGraphPanel_onHullReset, _S"Hull_Reset");
    ssuiListen(self->ui, self, HullGraphPanel_onHullRefresh, _S"Hull_Refresh");

    return parent_make();
}

void HullGraphPanel_clear(_In_ HullGraphPanel* self)
{
    string temp = 0;

    saClear(&self->sectoridx);
    IupSetAttribute(self->plot, "CLEAR", "1");

    // repair is the first dataset since it's in the background
    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        saPush(&self->sectoridx, int64, SPOINT(i, 0));
        strFromInt32(&temp, i, 10);
        IupPlotAddStr(self->plot, strC(temp), 0);
    }
    self->repairds = IupPlotEnd(self->plot);

    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_repair"));
    IupSetAttribute(self->plot, "DS_COLOR", "40 200 40");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARSPACING", "10");   // slightly wider
    // NOT stacked, no label

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[0] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_projectile"));
    IupSetAttribute(self->plot, "DS_COLOR", "200 40 40");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[1] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_missile"));
    IupSetAttribute(self->plot, "DS_COLOR", "160 160 160");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[2] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_beam"));
    IupSetAttribute(self->plot, "DS_COLOR", "100 100 240");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[3] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_asteroid"));
    IupSetAttribute(self->plot, "DS_COLOR", "40 200 200");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[4] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_solarflare"));
    IupSetAttribute(self->plot, "DS_COLOR", "200 120 120");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[5] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_asb"));
    IupSetAttribute(self->plot, "DS_COLOR", "200 40 200");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[6] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_systemdestroyed"));
    IupSetAttribute(self->plot, "DS_COLOR", "240 120 40");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[7] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_event"));
    IupSetAttribute(self->plot, "DS_COLOR", "200 200 0");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        IupPlotAddStr(self->plot, "", 0);
    }
    self->ds[8] = IupPlotEnd(self->plot);
    IupSetStrAttribute(self->plot, "DS_NAME", langGetC(self->ss, "damagesource_other"));
    IupSetAttribute(self->plot, "DS_COLOR", "255 170 255");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARLABEL", "YES");
    IupSetAttribute(self->plot, "DS_BARLABELFORMAT", "%.0f");
    IupSetAttribute(self->plot, "DS_BARSPACING", "40");
    IupSetAttribute(self->plot, "DS_BARSTACKED", "YES");

    strDestroy(&temp);
}

void HullGraphPanel_destroy(_In_ HullGraphPanel* self)
{
    // Autogen begins -----
    saDestroy(&self->sectoridx);
    // Autogen ends -------
}

// Autogen begins -----
// clang-format off
#include "ui/panel/hullgraph/hullgraphpanel.auto.inc"
// clang-format on
// Autogen ends -------
