// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "sectorinfo.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed SectorInfo* SectorInfo_create()
{
    SectorInfo* self;
    self = objInstCreate(SectorInfo);
    objInstInit(self);
    return self;
}

void SectorInfo_destroy(_In_ SectorInfo* self)
{
    // Autogen begins -----
    strDestroy(&self->type);
    // Autogen ends -------
}

intptr SectorInfo_cmp(_In_ SectorInfo* self, SectorInfo* other, uint32 flags)
{
    devAssert(objClsInfo(self) == objClsInfo(other));

    return self->sectorpoint - other->sectorpoint;
}

// Autogen begins -----
#include "sectorinfo.auto.inc"
// Autogen ends -------
