// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "infoblock.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed InfoBlock* InfoBlock_create(Subspace* ss)
{
    InfoBlock* self;
    self = objInstCreate(InfoBlock);

    self->ss = ss;
    strDup(&self->name, _S"InfoBlock");

    objInstInit(self);
    return self;
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void InfoBlock_applyDefaultSettings(_In_ InfoBlock* self)
{
    ssdLockedTransaction(self->settings)
    {
        if (!ssdPtr(self->settings, _S"ssvar"))
            ssdSet(self->settings, _S"ssver", false, stvar(int32, 0));
        if (!ssdPtr(self->settings, _S"ftlver"))
            ssdSet(self->settings, _S"ftlver", false, stvar(int32, 1));
        if (!ssdPtr(self->settings, _S"fps"))
            ssdSet(self->settings, _S"fps", false, stvar(int32, 2));
    }
}

// Autogen begins -----
#include "infoblock.auto.inc"
// Autogen ends -------
