// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/practicemode/practicemode.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed PracticeMode* PracticeMode_create(Subspace* ss)
{
    PracticeMode* self;
    self = objInstCreate(PracticeMode);

    self->ss = ss;
    strDup(&self->name, _S"PracticeMode");
    self->hidden = true;   // Hide while this is WIP

    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "feature/practicemode/practicemode.auto.inc"
// Autogen ends -------
