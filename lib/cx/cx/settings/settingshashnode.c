// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "settingshashnode.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "settings_private.h"

_objfactory_guaranteed SettingsHashNode* SettingsHashNode__create(SSDTree* tree)
{
    SettingsHashNode* self;
    self = objInstCreate(SettingsHashNode);

    self->tree = objAcquire(tree);

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool SettingsHashNode_init(_In_ SettingsHashNode* self)
{
    htInit(&self->binds, string, opaque(SettingsBind), 4, HT_Grow(At50));
    // Autogen begins -----
    return true;
    // Autogen ends -------
}

bool SettingsHashNode_bind(_In_ SettingsHashNode* self, _In_opt_ strref name, stype btyp,
                           void* bvar, stgeneric bdef, SSDLockState* _ssdCurrentLockState)
{
    ssdLockWrite(self);

    SettingsBind bind = { .type = btyp, .var = (stgeneric*)bvar, .def = bdef };

    // see if it exists in the main storage already
    stvar* svar = NULL;
    htelem elem = htFind(self->storage, strref, name, none, NULL);
    if (elem) {
        // found it, mark this as user-set
        bind.userset = true;
        svar         = hteValPtr(self->storage, stvar, elem);
    } else {
        // didn't already exist, set it to default
        stvar tmp = { .type = btyp, .data = bdef };
        elem      = htInsert(&self->storage, strref, name, stvar, tmp);
        svar      = hteValPtr(self->storage, stvar, elem);
    }

    // copy initial value from either settings or default
    _setsWriteBoundVar(&bind, svar->type, svar->data);
    _setsUpdateBindCache(&bind);

    htInsert(&self->binds, strref, name, opaque, bind);
    return true;
}

static void checkBound(SettingsHashNode* self, SettingsBind* bind, strref name,
                       SSDLockState* _ssdCurrentLockState)
{
    if (memcmp(&bind->cache, bind->var, _setsBCacheSize(bind->type)) != 0) {
        ssdLockWrite(self);   // upgrade to write lock

        // bound variable has changed! bring it into the value
        stvar nval = { .type = bind->type, .data = stStored(bind->type, bind->var) };
        htInsert(&self->storage, strref, name, stvar, nval);
        _setsUpdateBindCache(bind);
        ssdnodeUpdateModified(self);
    }
}

void SettingsHashNode_checkBound(_In_ SettingsHashNode* self, _In_opt_ strref name,
                                 SSDLockState* _ssdCurrentLockState)
{
    ssdLockRead(self);
    htelem elem = htFind(self->binds, strref, name, none, NULL);
    if (elem)
        checkBound(self, hteValPtr(self->binds, opaque, elem), name, _ssdCurrentLockState);
}

void SettingsHashNode_checkAll(_In_ SettingsHashNode* self, SSDLockState* _ssdCurrentLockState)
{
    ssdLockRead(self);

    // recurse to child hashtables
    foreach (hashtable, hti, self->storage) {
        stvar* val              = htiValPtr(stvar, hti);
        SettingsHashNode* child = stEq(val->type, stType(object)) ?
            objDynCast(SettingsHashNode, val->data.st_object) :
            NULL;
        if (child)
            settingshashnodeCheckAll(child, _ssdCurrentLockState);
    }

    foreach (hashtable, hti, self->binds) {
        checkBound(self,
                   (SettingsBind*)htiValPtr(opaque, hti),
                   htiKey(strref, hti),
                   _ssdCurrentLockState);
    }
}

static void destroyBinds(SettingsHashNode* self)
{
    foreach (hashtable, hti, self->binds) {
        SettingsBind* bind = htiValPtr(opaque, hti);
        // reset bound variable back to default
        _setsWriteBoundVar(bind, bind->type, bind->def);
        // destroy default in case it's a string
        _stDestroy(bind->type, NULL, &bind->def, 0);
    }
    htClear(&self->binds);
}

void SettingsHashNode_unbindAll(_In_ SettingsHashNode* self, SSDLockState* _ssdCurrentLockState)
{
    ssdLockWrite(self);
    // depth-first traversal
    foreach (hashtable, hti, self->storage) {
        stvar* val              = htiValPtr(stvar, hti);
        SettingsHashNode* child = stEq(val->type, stType(object)) ?
            objDynCast(SettingsHashNode, val->data.st_object) :
            NULL;
        if (child)
            settingshashnodeUnbindAll(child, _ssdCurrentLockState);
    }

    destroyBinds(self);
}

extern bool SSDHashNode_get(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out,
                            _Inout_ SSDLockState* _ssdCurrentLockState);   // parent
#define parent_get(idx, name, out, _ssdCurrentLockState) \
    SSDHashNode_get((SSDHashNode*)(self), idx, name, out, _ssdCurrentLockState)
bool SettingsHashNode_get(_In_ SettingsHashNode* self, int32 idx, _In_opt_ strref name, _When_(return == true, _Out_) stvar* out, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    SettingsHashNode_checkBound(self, name, _ssdCurrentLockState);
    return parent_get(idx, name, out, _ssdCurrentLockState);
}

extern bool SSDHashNode_set(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name, stvar val,
                            _Inout_ SSDLockState* _ssdCurrentLockState);   // parent
#define parent_set(idx, name, val, _ssdCurrentLockState) \
    SSDHashNode_set((SSDHashNode*)(self), idx, name, val, _ssdCurrentLockState)
bool SettingsHashNode_set(_In_ SettingsHashNode* self, int32 idx, _In_opt_ strref name, stvar val,
                          _Inout_ SSDLockState* _ssdCurrentLockState)
{
    stvar old;
    if (SettingsHashNode_get(self, idx, name, &old, _ssdCurrentLockState)) {
        if (stCmp(stvar, old, val, ST_Equality) == 0) {
            stvarDestroy(&old);
            // don't actually set it, this prevents modified timestamp from being updated
            return true;
        }
        stvarDestroy(&old);
    }

    bool ret = parent_set(idx, name, val, _ssdCurrentLockState);
    if (ret && idx == SSD_ByName) {
        // update binding if one exists
        htelem elem = htFind(self->binds, strref, name, none, NULL);
        if (elem) {
            SettingsBind* bind = hteValPtr(self->binds, opaque, elem);
            bind->userset      = true;
            _setsWriteBoundVar(bind, val.type, val.data);
            _setsUpdateBindCache(bind);
        }
    }
    return ret;
}

extern bool SSDHashNode_setC(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name,
                             _Inout_ stvar* val,
                             _Inout_ SSDLockState* _ssdCurrentLockState);   // parent
#define parent_setC(idx, name, val, _ssdCurrentLockState) \
    SSDHashNode_setC((SSDHashNode*)(self), idx, name, val, _ssdCurrentLockState)
bool SettingsHashNode_setC(_In_ SettingsHashNode* self, int32 idx, _In_opt_ strref name,
                           _Inout_ stvar* val, _Inout_ SSDLockState* _ssdCurrentLockState)
{
    stvar old;
    if (SettingsHashNode_get(self, idx, name, &old, _ssdCurrentLockState)) {
        if (stCmp(stvar, old, *val, ST_Equality) == 0) {
            stvarDestroy(&old);
            stvarDestroy(val);
            // don't actually set it, this prevents modified timestamp from being updated
            return true;
        }
        stvarDestroy(&old);
    }

    // have to do this unconditionally since parent_setC will destroy val
    ssdLockWrite(self);
    if (idx == SSD_ByName) {
        // update binding if one exists
        htelem elem = htFind(self->binds, strref, name, none, NULL);
        if (elem) {
            SettingsBind* bind = hteValPtr(self->binds, opaque, elem);
            bind->userset      = true;
            _setsWriteBoundVar(bind, val->type, val->data);
            _setsUpdateBindCache(bind);
        }
    }

    return parent_setC(idx, name, val, _ssdCurrentLockState);
}

extern bool SSDHashNode_remove(_In_ SSDHashNode* self, int32 idx, _In_opt_ strref name,
                               _Inout_ SSDLockState* _ssdCurrentLockState);   // parent
#define parent_remove(idx, name, _ssdCurrentLockState) \
    SSDHashNode_remove((SSDHashNode*)(self), idx, name, _ssdCurrentLockState)
bool SettingsHashNode_remove(_In_ SettingsHashNode* self, int32 idx, _In_opt_ strref name,
                             _Inout_ SSDLockState* _ssdCurrentLockState)
{
    ssdLockWrite(self);

    if (idx == SSD_ByName) {
        SettingsBind bind;
        // removing the value implicitly unbinds it
        if (htExtract(&self->binds, strref, name, opaque, &bind)) {
            // reset bound var back to default
            _setsWriteBoundVar(&bind, bind.type, bind.def);
        }
    }

    return parent_remove(idx, name, _ssdCurrentLockState);
}

void SettingsHashNode_destroy(_In_ SettingsHashNode* self)
{
    destroyBinds(self);
    // Autogen begins -----
    htDestroy(&self->binds);
    // Autogen ends -------
}

// Autogen begins -----
#include "settingshashnode.auto.inc"
// Autogen ends -------
