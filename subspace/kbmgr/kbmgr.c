// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "kbmgr.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "gamemgr/gamemgr.h"
#include "keymap.h"

_objfactory_guaranteed KBMgr* KBMgr_create(Subspace* subspace)
{
    KBMgr* self;
    self = objInstCreate(KBMgr);

    self->ss = subspace;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool KBMgr_init(_In_ KBMgr* self)
{
    // Autogen begins -----
    rwlockInit(&self->kbmgrlock);
    htInit(&self->binds, string, object, 16);
    return true;
    // Autogen ends -------
}

static int32 loadKeyBind(Subspace* ss, strref name)
{
    keymapInit();

    int32 ret = -1;

    string kpath = 0;
    strConcat(&kpath, _S"keybinds/", name);
    string kname = 0;
    if (ssdStringOut(ss->settings, kpath, &kname)) {
        if (strEmpty(kname))
            ret = 0;
        else
            htFind(keymap_strtoiup, string, kname, int32, &ret);
    }
    strDestroy(&kname);
    strDestroy(&kpath);

    return ret;
}

static void saveKeyBind(Subspace* ss, strref name, int32 iupkey)
{
    keymapInit();

    int32 ret = 0;

    string kpath = 0;
    strConcat(&kpath, _S"keybinds/", name);
    string kname = 0;
    if (iupkey == 0) {
        ssdSet(ss->settings, kpath, true, stvar(string, _S""));
    } else if (htFind(keymap_iuptostr, int32, iupkey, string, &kname)) {
        ssdSet(ss->settings, kpath, true, stvar(string, kname));
    }
    strDestroy(&kname);
    strDestroy(&kpath);
}

void KBMgr_reg(_In_ KBMgr* self, SubspaceFeature* owner, _In_opt_ strref name, int default_key)
{
    KeyBind* kb = keybindCreate(owner, name);

    // try to load saved setting
    kb->iupkey = loadKeyBind(self->ss, name);
    if (kb->iupkey == -1)
        kb->iupkey = default_key;

    htFind(keymap_iuptoftl, int32, kb->iupkey, int32, &kb->ftlkey);

    withWriteLock (&self->kbmgrlock) {
        htInsertC(&self->binds, string, kb->name, object, &kb);
    }
}

bool KBMgr_bind(_In_ KBMgr* self, _In_opt_ strref name, int key)
{
    bool ret = false;
    KeyBind* bind;

    // ensure that this is a valid key we know how to map to FTL
    if (key != 0 &&
        (!htHasKey(keymap_iuptoftl, int32, key) || !htHasKey(keymap_iuptodisplay, int32, key) ||
         !htHasKey(keymap_iuptostr, int32, key)))
        return false;

    rwlockAcquireWrite(&self->kbmgrlock);
    if (htFind(self->binds, strref, name, object, &bind)) {
        bind->iupkey = key;
        if (key == 0)
            bind->ftlkey = 0;
        else
            htFind(keymap_iuptoftl, int32, bind->iupkey, int32, &bind->ftlkey);
        rwlockReleaseWrite(&self->kbmgrlock);
        ret = true;

        saveKeyBind(self->ss, name, key);

        ControlClient* client = NULL;
        GameInst* ginst       = objAcquireFromWeak(GameInst, self->ss->curinst);
        if (ginst)
            client = objAcquireFromWeak(ControlClient, ginst->client);

        // if a client is connected, send an update
        if (client) {
            withReadLock (&self->kbmgrlock) {
                keybindSend(bind, client);
            }
        }

        objRelease(&client);
        objRelease(&ginst);
        objRelease(&bind);
    } else {
        rwlockReleaseWrite(&self->kbmgrlock);
    }

    return ret;
}

void KBMgr_sendAll(_In_ KBMgr* self, ControlClient* client)
{
    withReadLock (&self->kbmgrlock) {
        foreach (hashtable, hti, self->binds) {
            KeyBind* bind = (KeyBind*)htiVal(object, hti);
            keybindSend(bind, client);
        }
    }
}

void KBMgr_destroy(_In_ KBMgr* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->kbmgrlock);
    htDestroy(&self->binds);
    // Autogen ends -------
}

// Autogen begins -----
#include "kbmgr.auto.inc"
// Autogen ends -------
