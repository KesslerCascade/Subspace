// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/seededruns/seededruns.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed SeededRuns* SeededRuns_create(Subspace* ss)
{
    SeededRuns* self;
    self = objInstCreate(SeededRuns);

    self->ss = ss;
    strDup(&self->name, _S"SeededRuns");

    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "feature/seededruns/seededruns.auto.inc"
// Autogen ends -------
