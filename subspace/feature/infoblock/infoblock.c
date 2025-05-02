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

// Autogen begins -----
#include "infoblock.auto.inc"
// Autogen ends -------
