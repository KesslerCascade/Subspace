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
#include "gamemgr/gameinst.h"

_objfactory_guaranteed RunTracker* RunTracker_create(Subspace* ss)
{
    RunTracker* self;
    self = objInstCreate(RunTracker);

    self->ss = ss;
    strDup(&self->name, _S"RunTracker");

    objInstInit(self);
    return self;
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
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

extern void SubspaceFeature_enable(_In_ SubspaceFeature* self, bool enabled);   // parent
#define parent_enable(enabled) SubspaceFeature_enable((SubspaceFeature*)(self), enabled)
void RunTracker_enable(_In_ RunTracker* self, bool enabled)
{
    GameInst* inst = subspaceCurInst(self->ss);
    if (!inst || ginstGetState(inst) == GI_Menu) {
        // can change this if the game isn't running or is at the menu
        parent_enable(enabled);
        objRelease(&inst);
        return;
    }

    // otherwise do nothing; can't enable or disable
    objRelease(&inst);
    return;
}

// Autogen begins -----
#include "runtracker.auto.inc"
// Autogen ends -------
