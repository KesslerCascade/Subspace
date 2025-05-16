// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "tweaks.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "tweakspage.h"

_objfactory_guaranteed Tweaks* Tweaks_create(Subspace* ss)
{
    Tweaks* self;
    self = objInstCreate(Tweaks);

    self->ss = ss;
    strDup(&self->name, _S"Tweaks");

    objInstInit(self);
    return self;
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void Tweaks_applyDefaultSettings(_In_ Tweaks* self)
{
    int32 val;

    ssdLockedTransaction(self->settings)
    {
        if (!ssdPtr(self->settings, _S"targetfps"))
            ssdSet(self->settings, _S"targetfps", false, stvar(int32, 60));
    }
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
extern SettingsPage* SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* Tweaks_createSettingsPage(_In_ Tweaks* self, SubspaceUI* ui)
{
    return SettingsPage(tweakspageCreate(self, ui));
}

// Autogen begins -----
#include "tweaks.auto.inc"
// Autogen ends -------
