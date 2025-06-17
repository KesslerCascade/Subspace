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
#include "ui/subspaceui.h"
#include "ui/util/iupsetobj.h"

static intptr ScrapData_cmp(stype st, _In_ stgeneric gen1, _In_ stgeneric gen2, flags_t flags)
{
    ScrapData* s1 = (ScrapData*)gen1.st_opaque;
    ScrapData* s2 = (ScrapData*)gen2.st_opaque;

    return s1->sectorpoint - s2->sectorpoint;
}

STypeOps ScrapData_ops = { .cmp = ScrapData_cmp };

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
    saInit(&self->newscrap, opaque(ScrapData), 1);
    saInit(&self->sdata, custom(opaque(ScrapData), ScrapData_ops), 8, SA_Sorted);
    // Autogen begins -----
    mutexInit(&self->lock);
    return true;
    // Autogen ends -------
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool ScrapGraphPanel_make(_In_ ScrapGraphPanel* self)
{
    /*    Ihandle* lbl = IupLabel("No Data");
        IupSetAttribute(lbl, "FONT", "Helvetica, 12");
        IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
        IupSetAttribute(lbl, "EXPAND", "YES");
        IupSetAttribute(lbl, "ALIGNMENT", "ACENTER:ACENTER"); */
    self->plot = IupPlot();
    IupPlotBegin(self->plot, 1);
    // IupPlotAddStr(self->plot, "1", 55);
    // IupPlotAddStr(self->plot, "2", 103);
    self->ds    = IupPlotEnd(self->plot);
    self->si    = 0;
    self->reset = true;
    IupSetAttribute(self->plot, "FGCOLOR", "255 255 255");
    IupSetAttribute(self->plot, "AXS_YAUTOMIN", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKAUTO", "NO");
    IupSetAttribute(self->plot, "AXS_XTICKMINORDIVISION", "1");
    IupSetAttribute(self->plot, "AXS_YTIPFORMAT", "%.0f");
    IupSetAttribute(self->plot, "TIPFORMAT", "%.0s%.0s%s");

    self->h = IupBackgroundBox(self->plot);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);
    iupSetObj(self->h, self, ObjNone, self->ui);

    // register to receive all scrap events
    logrelaySubscribe(self->ss->runlog, self, _S"Scrap");

    return parent_make();
}

void ScrapGraphPanel_clear(_In_ ScrapGraphPanel* self)
{
    string temp = 0;

    saClear(&self->sdata);
    IupSetAttribute(self->plot, "CLEAR", "1");

    IupPlotBegin(self->plot, 1);
    for (int i = 1; i <= 8; i++) {
        ScrapData sd = { .amount = 0, .sectorpoint = SPOINT(i, 0) };
        saPush(&self->sdata, opaque, sd);
        strFromInt32(&temp, i, 10);
        IupPlotAddStr(self->plot, strC(temp), 0);
    }
    self->ds = IupPlotEnd(self->plot);
    self->si = 8;
    IupSetAttribute(self->plot, "DS_NAME", "Scrap");
    IupSetAttribute(self->plot, "DS_COLOR", "200 40 40");
    IupSetAttribute(self->plot, "DS_MODE", "BAR");
    IupSetAttribute(self->plot, "DS_BARSPACING", "30");
    self->reset  = false;
    self->redraw = true;

    strDestroy(&temp);
}

extern bool Panel_update(_In_ Panel* self);   // parent
#define parent_update() Panel_update((Panel*)(self))
bool ScrapGraphPanel_update(_In_ ScrapGraphPanel* self)
{
    string temp = 0;
    bool redraw = false;
    ScrapData* cur;

    withMutex (&self->lock) {
        if (self->reset)
            scrapgraphpanelClear(self);

        foreach (sarray, idx, ScrapData, ns, self->newscrap) {
            spointFormat(&temp, ns.sectorpoint);
            int32 idx = saFind(self->sdata, opaque, ns);   // this works because sdata custom
                                                           // compare only checks sectorpoint
            if (idx >= 0) {
                cur = &self->sdata.a[idx];
                cur->amount += ns.amount;
                IupPlotSetSampleStr(self->plot, self->ds, idx, strC(temp), cur->amount);
            } else {
                idx = saPush(&self->sdata, opaque, ns);
                IupPlotInsertStr(self->plot, self->ds, idx, strC(temp), ns.amount);
            }
        }
        saClear(&self->newscrap);

        redraw       = self->redraw;
        self->redraw = false;
    }

    if (redraw)
        IupSetAttribute(self->plot, "REDRAW", NULL);

    strDestroy(&temp);
    return true;
}

void ScrapGraphPanel_logNotify(_In_ ScrapGraphPanel* self, LogEnt* ent, bool replay)
{
    strref source = cfieldString(ent->params, _S"source");
    int amount    = cfieldValD(int32, ent->params, _S"amount", 0);

    if (strEq(source, _S"Event") && amount > 0) {
        // we're only interested in graphing scrap rewards

        ScrapData sdn = { .amount = amount, .sectorpoint = ent->sectorpoint };
        withMutex (&self->lock) {
            saPush(&self->newscrap, opaque, sdn);
            if (!replay)
                self->redraw = true;
        }

        // this function could be running in any thread; signal the UI thread to pick up the data
        // and process it
        ssuiUpdateMain(self->ss->ui, _S"scrapgraph");
    }
}

void ScrapGraphPanel_logReset(_In_ ScrapGraphPanel* self)
{
    withMutex (&self->lock) {
        saClear(&self->newscrap);
        self->reset = true;
    }

    ssuiUpdateMain(self->ss->ui, _S"scrapgraph");
}

void ScrapGraphPanel_logReplayComplete(_In_ ScrapGraphPanel* self)
{
    withMutex (&self->lock) {
        self->redraw = true;
    }

    ssuiUpdateMain(self->ss->ui, _S"scrapgraph");
}

void ScrapGraphPanel_destroy(_In_ ScrapGraphPanel* self)
{
    // Autogen begins -----
    mutexDestroy(&self->lock);
    saDestroy(&self->newscrap);
    saDestroy(&self->sdata);
    // Autogen ends -------
}

// Autogen begins -----
#include "scrapgraphpanel.auto.inc"
// Autogen ends -------
