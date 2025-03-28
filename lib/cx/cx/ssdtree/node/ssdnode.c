// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ssdnode.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/time/clock.h>

void SSDNode_destroy(_In_ SSDNode* self)
{
    // Autogen begins -----
    objRelease(&self->tree);
    // Autogen ends -------
}

bool SSDNode_isHashtable(_In_ SSDNode* self)
{
    return false;
}

bool SSDNode_isArray(_In_ SSDNode* self)
{
    return false;
}

void SSDNode_updateModified(_In_ SSDNode* self)
{
    self->modified = self->tree->modified = clockTimer();
}

_objinit_guaranteed bool SSDIterator_init(_In_ SSDIterator* self)
{
    if (!self->lstate)
        self->lstate = &self->transient_lock_state;
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

bool SSDIterator_isHashtable(_In_ SSDIterator* self)
{
    return false;
}

bool SSDIterator_isArray(_In_ SSDIterator* self)
{
    return false;
}

_Ret_opt_valid_ ObjInst* SSDIterator_objInst(_In_ SSDIterator* self)
{
    return stvarObjInst(self->_->ptr(self));
}

void SSDIterator_destroy(_In_ SSDIterator* self)
{
    if (self->transient_lock_state.init)
        _ssdLockEnd(self->node, &self->transient_lock_state);
    // Autogen begins -----
    objRelease(&self->node);
    // Autogen ends -------
}

// Autogen begins -----
#include "ssdnode.auto.inc"
// Autogen ends -------
