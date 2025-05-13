// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "timewarp.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "timewarppage.h"

_objfactory_guaranteed TimeWarp* TimeWarp_create(Subspace* ss)
{
    TimeWarp* self;
    self = objInstCreate(TimeWarp);

    self->ss = ss;
    strDup(&self->name, _S"TimeWarp");

    objInstInit(self);
    return self;
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void TimeWarp_applyDefaultSettings(_In_ TimeWarp* self)
{
    int32 val;

    ssdLockedTransaction(self->settings)
    {
        if (!ssdPtr(self->settings, _S"maxwarp"))
            ssdSet(self->settings, _S"maxwarp", false, stvar(int32, 32));
        if (!ssdPtr(self->settings, _S"doubletap"))
            ssdSet(self->settings, _S"doubletap", false, stvar(int32, 0));
        if (!ssdPtr(self->settings, _S"allowslowmo"))
            ssdSet(self->settings, _S"allowslowmo", false, stvar(bool, false));
    }
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) \
    SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* TimeWarp_createSettingsPage(_In_ TimeWarp* self, SubspaceUI* ui)
{
    return SettingsPage(timewarppageCreate(self, ui));
}

// Autogen begins -----
#include "timewarp.auto.inc"
// Autogen ends -------
