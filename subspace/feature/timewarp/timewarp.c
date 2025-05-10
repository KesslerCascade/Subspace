// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "timewarp.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed TimeWarp* TimeWarp_create(Subspace* ss)
{
    TimeWarp* self;
    self = objInstCreate(TimeWarp);

    self->ss = ss;
    strDup(&self->name, _S"TimeWarp");

    objInstInit(self);
    return self;
}

extern void SubspaceFeature_loadSettings(_In_ SubspaceFeature* self);   // parent
#define parent_loadSettings() SubspaceFeature_loadSettings((SubspaceFeature*)(self))
void TimeWarp_loadSettings(_In_ TimeWarp* self)
{
    int32 val;

    val = ssdVal(int32, self->ss->settings, _S"feature/TimeWarp/maxwarp", 32);
    htInsert(&self->settings, string, _S"maxwarp", stvar, stvar(int32, val));
    val = ssdVal(int32, self->ss->settings, _S"feature/TimeWarp/doubletap", 0);
    htInsert(&self->settings, string, _S"doubletap", stvar, stvar(int32, val));
    bool bval = ssdVal(bool, self->ss->settings, _S"feature/TimeWarp/allowslowmo", false);
    htInsert(&self->settings, string, _S"allowslowmo", stvar, stvar(bool, bval));
}

// Autogen begins -----
#include "timewarp.auto.inc"
// Autogen ends -------
