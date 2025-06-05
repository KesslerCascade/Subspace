// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "savemanager.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed SaveManager* SaveManager_create(Subspace* ss)
{
    SaveManager* self;
    self = objInstCreate(SaveManager);

    self->ss = ss;
    strDup(&self->name, _S"SaveManager");

    objInstInit(self);
    return self;
}

extern SettingsPage* SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* SaveManager_createSettingsPage(_In_ SaveManager* self, SubspaceUI* ui)
{
    return NULL;
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void SaveManager_applyDefaultSettings(_In_ SaveManager* self)
{
    return;
}

// Autogen begins -----
#include "savemanager.auto.inc"
// Autogen ends -------
