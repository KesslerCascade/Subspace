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

extern void SubspaceFeature_loadSettings(_In_ SubspaceFeature* self);   // parent
#define parent_loadSettings() SubspaceFeature_loadSettings((SubspaceFeature*)(self))
void InfoBlock_loadSettings(_In_ InfoBlock* self)
{
    int32 val;

    val = ssdVal(int32, self->ss->settings, _S"feature/InfoBlock/ssver", 0);
    htInsert(&self->settings, string, _S"ssver", stvar, stvar(int32, val));
    val = ssdVal(int32, self->ss->settings, _S"feature/InfoBlock/ftlver", 1);
    htInsert(&self->settings, string, _S"ftlver", stvar, stvar(int32, val));
    val = ssdVal(int32, self->ss->settings, _S"feature/InfoBlock/fps", 2);
    htInsert(&self->settings, string, _S"fps", stvar, stvar(int32, val));
}

// Autogen begins -----
#include "infoblock.auto.inc"
// Autogen ends -------
