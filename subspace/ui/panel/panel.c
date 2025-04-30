// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "panel.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/subspaceui.h"

const char* panelbg = "32 32 32";

void Panel_destroy(_In_ Panel* self)
{
    // Autogen begins -----
    objDestroyWeak(&self->ui);
    strDestroy(&self->title);
    // Autogen ends -------
}

bool Panel_make(_In_ Panel* self)
{
    if (self->h) {
        IupSetAttribute(self->h, "CXOBJ", (const char*)self);
        if (!strEmpty(self->title))
            IupSetAttribute(self->h, "TABTITLE", strC(self->title));
    }
    return true;
}

bool Panel_update(_In_ Panel* self)
{
    return true;
}

intptr Panel_cmp(_In_ Panel* self, Panel* other, uint32 flags)
{
    return stCmp(strref, self->name, other->name);
}

// Autogen begins -----
#include "panel.auto.inc"
// Autogen ends -------
