// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/numerichull/numerichull.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed NumericHull* NumericHull_create(Subspace* ss)
{
    NumericHull* self;
    self = objInstCreate(NumericHull);

    self->ss = ss;
    strDup(&self->name, _S"NumericHull");

    objInstInit(self);
    return self;
}

// Autogen begins -----
// clang-format off
#include "feature/numerichull/numerichull.auto.inc"
// clang-format on
// Autogen ends -------
