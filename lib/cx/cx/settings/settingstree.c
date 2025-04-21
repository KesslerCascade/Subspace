// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "settingstree.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "settings_private.h"
#include "settingshashnode.h"

_objfactory_guaranteed SettingsTree* SettingsTree_create()
{
    SettingsTree* self;
    self = objInstCreate(SettingsTree);

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool SettingsTree_init(_In_ SettingsTree* self)
{
    self->interval                        = SETTINGS_DEFAULT_FLUSH_INTERVAL;
    self->factories[SSD_Create_Hashtable] = (SSDNodeFactory)SettingsHashNode__create;
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

void SettingsTree_destroy(_In_ SettingsTree* self)
{
    // Autogen begins -----
    objRelease(&self->vfs);
    strDestroy(&self->filename);
    // Autogen ends -------
}

// Autogen begins -----
#include "settingstree.auto.inc"
// Autogen ends -------
