// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "infoblockpage.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed InfoBlockPage* InfoBlockPage_create(InfoBlock* feature, SubspaceUI* ui)
{
    InfoBlockPage* self;
    self = objInstCreate(InfoBlockPage);

    self->feature = feature;
    self->ui      = ui;
    self->ss      = ui->ss;
    self->visible = true;

    self->name = _S"infoblock";
    strDup(&self->title, langGet(self->ss, _S"feature_infoblock"));
    strDup(&self->imgname, _S"IMAGE_INFOBLOCK");

    objInstInit(self);

    return self;
}

extern bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list);   // parent
#define parent_make(list) SettingsPage_make((SettingsPage*)(self), list)
bool InfoBlockPage_make(_In_ InfoBlockPage* self, Ihandle* list)
{
    self->h = IupFill();

    iupLoadImage(self->ss, _S"IMAGE_INFOBLOCK", _S"svg", _S"subspace:/infoblock.svg", list);

    return true;
}

extern bool SettingsPage_update(_In_ SettingsPage* self);   // parent
#define parent_update() SettingsPage_update((SettingsPage*)(self))
bool InfoBlockPage_update(_In_ InfoBlockPage* self)
{
    return true;
}

// Autogen begins -----
#include "infoblockpage.auto.inc"
// Autogen ends -------
