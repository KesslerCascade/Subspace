// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "frameadv.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed FrameAdv* FrameAdv_create(Subspace* ss)
{
    FrameAdv* self;
    self = objInstCreate(FrameAdv);

    self->ss = ss;
    strDup(&self->name, _S"FrameAdv");

    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "frameadv.auto.inc"
// Autogen ends -------
