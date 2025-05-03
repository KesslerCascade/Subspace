// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "optionspage.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/util/iupsetobj.h"

bool OptionsPage_make(_In_ OptionsPage* self, Ihandle* list)
{
    if (self->h) {
        iupSetObj(self->h, self, ObjNone, self->ui);
    }
    return true;
}

bool OptionsPage_update(_In_ OptionsPage* self)
{
    return true;
}

void OptionsPage_destroy(_In_ OptionsPage* self)
{
    // Autogen begins -----
    strDestroy(&self->imgname);
    strDestroy(&self->title);
    // Autogen ends -------
}

// Autogen begins -----
#include "optionspage.auto.inc"
// Autogen ends -------
