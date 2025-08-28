// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/screenshot/screenshot.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <iupkey.h>
#include "feature/screenshot/screenshotpage.h"
#include "kbmgr/kbmgr.h"

_objfactory_guaranteed Screenshot* Screenshot_create(Subspace* ss)
{
    Screenshot* self;
    self = objInstCreate(Screenshot);

    self->ss = ss;
    strDup(&self->name, _S"Screenshot");

    kbmgrReg(ss->kbmgr, self, _S"screenshot_take", K_F12);

    objInstInit(self);
    return self;
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) \
    SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* Screenshot_createSettingsPage(_In_ Screenshot* self, SubspaceUI* ui)
{
    return SettingsPage(screenshotpageCreate(self, ui));
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void Screenshot_applyDefaultSettings(_In_ Screenshot* self)
{
    ssdLockedTransaction(self->settings)
    {
        if (!ssdPtr(self->settings, _S"hidemouse"))
            ssdSet(self->settings, _S"hidemouse", false, stvar(bool, true));
        if (!ssdPtr(self->settings, _S"hidepause"))
            ssdSet(self->settings, _S"hidepause", false, stvar(bool, false));
        if (!ssdPtr(self->settings, _S"filename"))
            ssdSet(self->settings, _S"filename", true, stvar(string, SCREENSHOTNAME_DEFAULT));
    }
}

// Autogen begins -----
#include "feature/screenshot/screenshot.auto.inc"
// Autogen ends -------
