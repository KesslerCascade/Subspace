// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "runtracker.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed RunTracker* RunTracker_create(Subspace* ss)
{
    RunTracker* self;
    self = objInstCreate(RunTracker);

    self->ss = ss;
    strDup(&self->name, _S"RunTracker");

    objInstInit(self);
    return self;
}

extern SettingsPage* SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* RunTracker_createSettingsPage(_In_ RunTracker* self, SubspaceUI* ui)
{
    return NULL;
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void RunTracker_applyDefaultSettings(_In_ RunTracker* self)
{
    return;
}

// Autogen begins -----
#include "runtracker.auto.inc"
// Autogen ends -------
