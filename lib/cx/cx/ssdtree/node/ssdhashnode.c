// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "ssdhashnode.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "../ssdtree.h"

_objfactory_guaranteed SSDHashNode* SSDHashNode__create(SSDTree* tree)
{
    SSDHashNode *self;
    self = objInstCreate(SSDHashNode);

    self->tree = objAcquire(tree);
    ssdnodeUpdateModified(self);

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool SSDHashNode_init(_In_ SSDHashNode* self)
{
    flags_t htflags = 0;
    if (self->tree->flags & SSD_CaseInsensitive)
        htflags = HT_CaseInsensitive;
    htInit(&self->storage, string, stvar, 16, htflags);

    // Autogen begins -----
    return true;
    // Autogen ends -------
}

bool SSDHashNode_get(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    bool ret = false;

    if (idx != SSD_ByName)
        goto out;

    ssdLockRead(self);
    ret = htFind(self->storage, strref, name, stvar, out);

out:
    if (!ret)
        *out = stvNone;
    return ret;
}

_Ret_opt_valid_ stvar* SSDHashNode_ptr(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    if (idx != SSD_ByName)
        return NULL;

    ssdLockRead(self);

    htelem elem = htFind(self->storage, strref, name, none, NULL);
    if (elem)
        return hteValPtr(self->storage, stvar, elem);

    return NULL;
}

bool SSDHashNode_set(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, stvar val, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    if (idx != SSD_ByName)
        return false;

    ssdLockWrite(self);
    htInsert(&self->storage, strref, name, stvar, val);
    ssdnodeUpdateModified(self);
    return true;
}

bool SSDHashNode_setC(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, _Inout_ stvar* val, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    if (idx != SSD_ByName) {
        stvarDestroy(val);
        return false;
    }

    ssdLockWrite(self);
    htInsertC(&self->storage, strref, name, stvar, val);
    *val = stvNone;
    ssdnodeUpdateModified(self);
    return true;
}

bool SSDHashNode_remove(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    if (idx != SSD_ByName)
        return false;

    ssdLockWrite(self);
    bool ret = htRemove(&self->storage, strref, name);
    if (ret)
        ssdnodeUpdateModified(self);
    return ret;
}

_Ret_valid_ SSDIterator* SSDHashNode_iter(_In_ SSDHashNode* self)
{
    SSDHashIter *ret = ssdhashiterCreate(self, NULL);
    return SSDIterator(ret);
}

SSDIterator* SSDHashNode__iterLocked(_In_ SSDHashNode* self, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    SSDHashIter *ret = ssdhashiterCreate(self, _ssdCurrentLockState);
    return SSDIterator(ret);
}

int32 SSDHashNode_count(_In_ SSDHashNode* self, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockRead(self);
    return htSize(self->storage);
}

extern bool SSDNode_isHashtable(_In_ SSDNode* self);   // parent
#define parent_isHashtable() SSDNode_isHashtable((SSDNode*)(self))
bool SSDHashNode_isHashtable(_In_ SSDHashNode* self)
{
    return true;
}

void SSDHashNode_destroy(_In_ SSDHashNode* self)
{
    // Autogen begins -----
    htDestroy(&self->storage);
    // Autogen ends -------
}

// -------------------------------- ITERATOR --------------------------------

_objfactory_guaranteed SSDHashIter* SSDHashIter_create(SSDHashNode* node, SSDLockState* lstate)
{
    SSDHashIter *self;
    self = objInstCreate(SSDHashIter);

    self->node = (SSDNode*)objAcquire(node);
    self->lstate = lstate;

    objInstInit(self);

    // grab iterator after object init because base SSDIterator will lock the tree
    // for read access during the above call to objInstInit
    htiInit(&self->iter, ((SSDHashNode*)self->node)->storage);

    return self;
}

bool SSDHashIter_valid(_In_ SSDHashIter* self)
{
    _ssdManualLockRead(self->node, self->lstate);
    bool ret = htiValid(&self->iter);
    _ssdUnlock(self->node, &self->transient_lock_state);
    return ret;
}

bool SSDHashIter_next(_In_ SSDHashIter* self)
{
    _ssdManualLockRead(self->node, self->lstate);
    bool ret = htiNext(&self->iter);
    _ssdUnlock(self->node, &self->transient_lock_state);
    return ret;
}

stvar* SSDHashIter_ptr(_In_ SSDHashIter* self)
{
    _ssdManualLockRead(self->node, self->lstate);
    // this does not unlock the transient lock!
    // you really shouldn't be using ptr() with the transient lock anyway...
    devAssert(!self->transient_lock_state.init);

    if (!htiValid(&self->iter))
        return NULL;

    return htiValPtr(stvar, self->iter);
}

bool SSDHashIter_get(_In_ SSDHashIter* self, stvar* out)
{
    bool ret = false;
    _ssdManualLockRead(self->node, self->lstate);

    if (htiValid(&self->iter)) {
        stvarCopy(out, htiVal(stvar, self->iter));
        ret = true;
    }

    _ssdUnlock(self->node, &self->transient_lock_state);
    return ret;
}

int32 SSDHashIter_idx(_In_ SSDHashIter* self)
{
    return SSD_ByName;
}

strref SSDHashIter_name(_In_ SSDHashIter* self)
{
    strref ret = NULL;
    _ssdManualLockRead(self->node, self->lstate);

    if (htiValid(&self->iter)) {
        // Have to duplicate this into the iterator because it would be dangerous to
        // return the reference directly to the hash key without the lock held,
        // which is the case if the transient lock is being used. Thankfully refcounted
        // strings make this fairly cheap.
        strDup(&self->lastName, htiKey(strref, self->iter));
        ret = self->lastName;
    }

    _ssdUnlock(self->node, &self->transient_lock_state);
    return ret;
}

bool SSDHashIter_iterOut(_In_ SSDHashIter* self, _When_(return == true, _Out_) int32* idx, _When_(return == true, _Out_) strref* name, _When_(return == true, _Out_) stvar** val)
{
    _ssdManualLockRead(self->node, self->lstate);
    // this shouldn't be used in transient lock mode
    devAssert(!self->transient_lock_state.init);

    if (!htiValid(&self->iter))
        return false;

    *idx = SSD_ByName;
    *name = htiKey(strref, self->iter);
    *val = htiValPtr(stvar, self->iter);

    return true;
}

void SSDHashIter_destroy(_In_ SSDHashIter* self)
{
    htiFinish(&self->iter);
    // Autogen begins -----
    strDestroy(&self->lastName);
    // Autogen ends -------
}

extern bool SSDIterator_isHashtable(_In_ SSDIterator* self);   // parent
#undef parent_isHashtable
#define parent_isHashtable() SSDIterator_isHashtable((SSDIterator*)(self))
bool SSDHashIter_isHashtable(_In_ SSDHashIter* self)
{
    return true;
}

// Autogen begins -----
#include "ssdhashnode.auto.inc"
// Autogen ends -------
