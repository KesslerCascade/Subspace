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
    htInit(&self->settings, string, stvar, 16);
    return true;
    // Autogen ends -------
}

void SubspaceFeature_destroy(_In_ SubspaceFeature* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    strDestroy(&self->name);
    htDestroy(&self->settings);
    // Autogen ends -------
}

_objfactory_guaranteed ClientFeature* ClientFeature_create(_In_opt_ strref name)
{
    ClientFeature* self;
    self = objInstCreate(ClientFeature);

    strDup(&self->name, name);

    objInstInit(self);
    return self;
}

SettingsPage* SubspaceFeature_getSettingsPage(_In_ SubspaceFeature* self)
{
    return NULL;
}

void ClientFeature_destroy(_In_ ClientFeature* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    // Autogen ends -------
}

void SubspaceFeature_enable(_In_ SubspaceFeature* self, bool enabled)
{
    return;
}

// Autogen begins -----
#include "feature.auto.inc"
// Autogen ends -------
