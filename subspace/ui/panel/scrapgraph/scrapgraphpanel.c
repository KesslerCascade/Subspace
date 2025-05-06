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
#include "ui/subspaceui.h"

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

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool ScrapGraphPanel_make(_In_ ScrapGraphPanel* self)
{
    Ihandle* lbl = IupLabel("No Data");
    IupSetAttribute(lbl, "FONT", "Helvetica, 12");
    IupSetAttribute(lbl, "FGCOLOR", "255 255 255");
    IupSetAttribute(lbl, "EXPAND", "YES");
    IupSetAttribute(lbl, "ALIGNMENT", "ACENTER:ACENTER");

    self->h = IupBackgroundBox(lbl);
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    return parent_make();
}

// Autogen begins -----
#include "scrapgraphpanel.auto.inc"
// Autogen ends -------
