// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "settingspage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/util/iupsetobj.h"

bool SettingsPage_make(_In_ SettingsPage* self, Ihandle* list)
{
    if (self->h) {
        iupSetObj(self->h, self, ObjNone, self->ui);
    }
    return true;
}

bool SettingsPage_update(_In_ SettingsPage* self)
{
    return true;
}

void SettingsPage_destroy(_In_ SettingsPage* self)
{
    // Autogen begins -----
    strDestroy(&self->imgname);
    strDestroy(&self->title);
    // Autogen ends -------
}

// Autogen begins -----
#include "settingspage.auto.inc"
// Autogen ends -------
