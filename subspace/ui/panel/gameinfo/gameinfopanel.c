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
#include "ui/subspaceui.h"

_objfactory_guaranteed GameInfoPanel* GameInfoPanel_create(SubspaceUI* ui)
{
    GameInfoPanel* self;
    self = objInstCreate(GameInfoPanel);
    self->ss = ui->ss;
    self->ui   = objGetWeak(SubspaceUI, ui);
    strDup(&self->title, _S"Game Info");
    self->name = _S"gameinfo";
    objInstInit(self);
    return self;
}

extern bool Panel_make(_In_ Panel* self);   // parent
#define parent_make() Panel_make((Panel*)(self))
bool GameInfoPanel_make(_In_ GameInfoPanel* self)
{
    self->h = IupBackgroundBox(IupVbox(IupHbox(IupFill(), NULL), IupFill(), NULL));
    IupSetAttribute(self->h, "BGCOLOR", panelbg);

    return parent_make();
}

// Autogen begins -----
#include "gameinfopanel.auto.inc"
// Autogen ends -------
