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
#include <iupkey.h>
#include "kbmgr/kbmgr.h"

_objfactory_guaranteed PracticeMode* PracticeMode_create(Subspace* ss)
{
    PracticeMode* self;
    self = objInstCreate(PracticeMode);

    self->ss = ss;
    strDup(&self->name, _S"PracticeMode");

    // kbmgrReg(ss->kbmgr, self, _S"practicemode_cheats", K_F9);
    kbmgrReg(ss->kbmgr, self, _S"practicemode_revert", K_F10);

    objInstInit(self);
    return self;
}

// Autogen begins -----
// clang-format off
#include "feature/practicemode/practicemode.auto.inc"
// clang-format on
// Autogen ends -------
