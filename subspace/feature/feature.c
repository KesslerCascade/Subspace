// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objinit_guaranteed bool SubspaceFeature_init(_In_ SubspaceFeature* self)
{
    // Autogen begins -----
    rwlockInit(&self->lock);
    return true;
    // Autogen ends -------
}

void SubspaceFeature_destroy(_In_ SubspaceFeature* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    // Autogen ends -------
}

// Autogen begins -----
#include "feature.auto.inc"
// Autogen ends -------
