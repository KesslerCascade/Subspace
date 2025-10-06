// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ui/panel/runsummary/runsummarypanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/subspaceui.h"

_objfactory_guaranteed RunSummaryPanel* RunSummaryPanel_create(SubspaceUI* ui)
{
    RunSummaryPanel* self;
    self = objInstCreate(RunSummaryPanel);

    self->ss = ui->ss;
    self->ui = ui;
    strDup(&self->title, langGet(self->ss, _S"panel_runsummary"));
    self->name = _S"runsummary";

    objInstInit(self);
    return self;
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool RunSummaryPanel_make(_In_ RunSummaryPanel* self)
{
    self->h = IupBackgroundBox(IupVbox(IupHbox(IupFill(), NULL), IupFill(), NULL));
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    return parent_make();
}

// Autogen begins -----
// clang-format off
#include "ui/panel/runsummary/runsummarypanel.auto.inc"
// clang-format on
// Autogen ends -------
