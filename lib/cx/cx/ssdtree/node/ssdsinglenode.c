// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ssdsinglenode.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "../ssdtree.h"

_objfactory_guaranteed SSDSingleNode* SSDSingleNode__create(SSDTree* tree)
{
    SSDSingleNode *self;
    self = objInstCreate(SSDSingleNode);

    self->tree = objAcquire(tree);

    objInstInit(self);
    return self;
}

bool SSDSingleNode_get(_In_ SSDSingleNode* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockRead(self);
    stvarCopy(out, self->storage);
    return true;
}

_Ret_opt_valid_ stvar* SSDSingleNode_ptr(_In_ SSDSingleNode* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockRead(self);
    return &self->storage;
}

bool SSDSingleNode_set(_In_ SSDSingleNode* self, int32 idx, _In_opt_ strref name, stvar val, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockWrite(self);
    stvarDestroy(&self->storage);
    stvarCopy(&self->storage, val);
    ssdnodeUpdateModified(self);
    return true;
}

bool SSDSingleNode_setC(_In_ SSDSingleNode* self, int32 idx, _In_opt_ strref name, _Inout_ stvar* val, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockWrite(self);
    stvarDestroy(&self->storage);
    self->storage = *val;
    *val = stvNone;
    ssdnodeUpdateModified(self);
    return true;
}

bool SSDSingleNode_remove(_In_ SSDSingleNode* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    // can't remove a single node!
    return false;
}

_Ret_valid_ SSDIterator* SSDSingleNode_iter(_In_ SSDSingleNode* self)
{
    SSDSingleIter *iter = ssdsingleiterCreate(self, NULL);
    return SSDIterator(iter);
}

SSDIterator* SSDSingleNode__iterLocked(_In_ SSDSingleNode* self, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    SSDSingleIter *iter = ssdsingleiterCreate(self, _ssdCurrentLockState);
    return SSDIterator(iter);
}

int32 SSDSingleNode_count(_In_ SSDSingleNode* self, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    return 1;
}

void SSDSingleNode_destroy(_In_ SSDSingleNode* self)
{
    // Autogen begins -----
    _stDestroy(self->storage.type, NULL, &self->storage.data, 0);
    // Autogen ends -------
}

// -------------------------------- ITERATOR --------------------------------

_objfactory_guaranteed SSDSingleIter* SSDSingleIter_create(SSDSingleNode* node, SSDLockState* lstate)
{
    SSDSingleIter *self;
    self = objInstCreate(SSDSingleIter);

    self->node = (SSDNode*)objAcquire(node);
    self->lstate = lstate;

    objInstInit(self);
    return self;
}

bool SSDSingleIter_valid(_In_ SSDSingleIter* self)
{
    return !self->done;
}

bool SSDSingleIter_next(_In_ SSDSingleIter* self)
{
    self->done = true;
    return true;
}

stvar* SSDSingleIter_ptr(_In_ SSDSingleIter* self)
{
    if (self->done)
        return NULL;

    _ssdManualLockRead(self->node, self->lstate);
    // this shouldn't be used in transient lock mode
    devAssert(!self->transient_lock_state.init);

    return &((SSDSingleNode*)self->node)->storage;
}

bool SSDSingleIter_get(_In_ SSDSingleIter* self, stvar* out)
{
    if (self->done)
        return false;

    _ssdManualLockRead(self->node, self->lstate);
    stvarCopy(out, ((SSDSingleNode *)self->node)->storage);

    _ssdUnlock(self->node, &self->transient_lock_state);
    return true;
}

int32 SSDSingleIter_idx(_In_ SSDSingleIter* self)
{
    return 0;
}

strref SSDSingleIter_name(_In_ SSDSingleIter* self)
{
    return _S"0";
}

bool SSDSingleIter_iterOut(_In_ SSDSingleIter* self, _When_(return == true, _Out_) int32* idx, _When_(return == true, _Out_) strref* name, _When_(return == true, _Out_) stvar** val)
{
    if (self->done)
        return false;

    _ssdManualLockRead(self->node, self->lstate);
    // this shouldn't be used in transient lock mode
    devAssert(!self->transient_lock_state.init);

    *idx = 0;
    *name = _S"0";
    *val = &((SSDSingleNode *)self->node)->storage;
    return true;
}

// Autogen begins -----
#include "ssdsinglenode.auto.inc"
// Autogen ends -------
