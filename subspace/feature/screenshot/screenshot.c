// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/screenshot/screenshot.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <iupkey.h>
#include "kbmgr/kbmgr.h"

_objfactory_guaranteed Screenshot* Screenshot_create(Subspace* ss)
{
    Screenshot* self;
    self = objInstCreate(Screenshot);

    self->ss = ss;
    strDup(&self->name, _S"Screenshot");

    kbmgrReg(ss->kbmgr, self, _S"screenshot_take", K_F12);

    objInstInit(self);
    return self;
}

// Autogen begins -----
#include "feature/screenshot/screenshot.auto.inc"
// Autogen ends -------
