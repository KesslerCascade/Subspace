// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "notableeventpanel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/subspaceui.h"

_objfactory_guaranteed NotableEventPanel* NotableEventPanel_create(SubspaceUI* ui)
{
    NotableEventPanel* self;
    self = objInstCreate(NotableEventPanel);

    self->ss = ui->ss;
    self->ui = ui;
    strDup(&self->title, langGet(self->ss, _S"panel_notableevent"));
    self->name = _S"notableevent";

    objInstInit(self);
    return self;
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool NotableEventPanel_make(_In_ NotableEventPanel* self)
{
    self->h = IupBackgroundBox(IupVbox(IupHbox(IupFill(), NULL), IupFill(), NULL));
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    return parent_make();
}

// Autogen begins -----
#include "notableeventpanel.auto.inc"
// Autogen ends -------
