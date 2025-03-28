// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ssdtreeobj.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ssdnodes.h"

_objfactory_guaranteed SSDTree* SSDTree_create(uint32 flags)
{
    SSDTree *self;
    self = objInstCreate(SSDTree);

    self->flags = flags;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool SSDTree_init(_In_ SSDTree* self)
{
    // default factories
    if (!self->factories[SSD_Create_Hashtable])
        self->factories[SSD_Create_Hashtable] = (SSDNodeFactory)SSDHashNode__create;
    if (!self->factories[SSD_Create_Array])
        self->factories[SSD_Create_Array] = (SSDNodeFactory)SSDArrayNode__create;
    if (!self->factories[SSD_Create_Single])
        self->factories[SSD_Create_Single] = (SSDNodeFactory)SSDSingleNode__create;

#ifdef SSD_LOCK_DEBUG
    mutexInit(&self->dbg.mtx);
    saInit(&self->dbg.readlocks, opaque(SSDLockDebug), 16);
    saInit(&self->dbg.writelocks, opaque(SSDLockDebug), 16);
#endif

    // Autogen begins -----
    rwlockInit(&self->lock);
    return true;
    // Autogen ends -------
}

_objfactory_guaranteed SSDNode* SSDTree_createNode(_In_ SSDTree* self, _In_range_(SSD_Create_None+1, SSD_Create_Count-1) SSDCreateType crtype)
{
    devAssert(crtype > SSD_Create_None && crtype < SSD_Create_Count);

    bool valid = crtype > SSD_Create_None && crtype < SSD_Create_Count && self->factories[crtype];

    _Analysis_assume_(valid == true);
    if (!valid)
        return NULL;

    return self->factories[crtype](self);
}

void SSDTree_destroy(_In_ SSDTree* self)
{
#ifdef SSD_LOCK_DEBUG
    saDestroy(&self->dbg.readlocks);
    saDestroy(&self->dbg.writelocks);
    mutexDestroy(&self->dbg.mtx);
#endif
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    // Autogen ends -------
}

// Autogen begins -----
#include "ssdtreeobj.auto.inc"
// Autogen ends -------
