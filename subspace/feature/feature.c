// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature/feature.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "control/controlclient.h"
#include "gamemgr/gamemgr.h"
#include "ui/page/settingspage.h"

_objinit_guaranteed bool SubspaceFeature_init(_In_ SubspaceFeature* self)
{
    string spath = 0;
    strNConcat(&spath, _S"feature/", self->name, _S"/settings");
    self->settings = ssdSubtree(self->ss->settings, spath, SSD_Create_Hashtable);
    strDestroy(&spath);
    // Autogen begins -----
    rwlockInit(&self->lock);
    return true;
    // Autogen ends -------
}

void SubspaceFeature_destroy(_In_ SubspaceFeature* self)
{
    // Autogen begins -----
    rwlockDestroy(&self->lock);
    strDestroy(&self->name);
    objRelease(&self->settings);
    // Autogen ends -------
}

_objfactory_guaranteed ClientFeature* ClientFeature_create(_In_opt_ strref name)
{
    ClientFeature* self;
    self = objInstCreate(ClientFeature);

    strDup(&self->name, name);

    objInstInit(self);
    return self;
}

SettingsPage* SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui)
{
    return NULL;
}

void ClientFeature_destroy(_In_ ClientFeature* self)
{
    // Autogen begins -----
    strDestroy(&self->name);
    // Autogen ends -------
}

void SubspaceFeature_enable(_In_ SubspaceFeature* self, bool enabled)
{
    withWriteLock (&self->lock) {
        self->enabled = enabled;
    }

    // send feature state and config (if enabled) to any connected clients
    GameMgr* gmgr = self->ss->gmgr;
    withReadLock (&gmgr->gmgrlock) {
        foreach (hashtable, hti, gmgr->insts) {
            GameInst* inst        = (GameInst*)htiVal(object, hti);
            ControlClient* client = objAcquireFromWeak(ControlClient, inst->client);
            if (!client)
                continue;

            if (enabled)
                featureSendAllSettings(self, client);

            ControlMsg* emsg = controlMsgCreate(_S"EnableFeature");
            cfieldSet(emsg, _S"feature", string, self->name);
            cfieldSet(emsg, _S"enabled", bool, enabled);
            cclientQueue(client, emsg);

            objRelease(&client);
        }
    }

    // save state in settings
    string epath = 0;
    strNConcat(&epath, _S"feature/", self->name, _S"/enabled");
    ssdSet(self->ss->settings, epath, true, stvar(bool, enabled));
    strDestroy(&epath);

    // update entire UI when feature state changes
    subspaceUpdateUI(self->ss);
    return;
}

void SubspaceFeature_setAvailable(_In_ SubspaceFeature* self, bool available)
{
    withWriteLock (&self->lock) {
        self->available = available;
    }

    // save state in settings
    string epath = 0;
    strNConcat(&epath, _S"feature/", self->name, _S"/available");
    ssdSet(self->ss->settings, epath, true, stvar(bool, available));
    strDestroy(&epath);
}

static void sendSettingVal(ControlMsg* msg, strref fname, stvar* val)
{
    if (stvarIs(val, int64)) {
        // parsed from json... have to figure out whether it's signed or unsigned int32
        if (val->data.st_int64 >= INT32_MIN && val->data.st_int64 <= INT32_MAX) {
            cfieldSet(msg, fname, int32, (int32)val->data.st_int64);
        } else if (val->data.st_int64 >= 0 && val->data.st_int64 <= UINT32_MAX) {
            cfieldSet(msg, fname, uint32, (uint32)val->data.st_int64);
        } else {
            cfieldSet(msg, fname, int64, val->data.st_int64);
        }
    } else if (STYPE_CLASS(stGetId(val->type)) == STCLASS_INT ||
               STYPE_CLASS(stGetId(val->type)) == STCLASS_UINT ||
               STYPE_CLASS(stGetId(val->type)) == STCLASS_FLOAT || stvarIs(val, string)) {
        // for other basic types just send as-is
        htInsert(&msg->fields, strref, fname, stvar, *val);
    } else {
        return;
    }

    // do not send hashtables, objects, or other complex types
}

void SubspaceFeature_sendSetting(_In_ SubspaceFeature* self, ControlClient* client,
                                 _In_opt_ strref name)
{
    stvar val;

    if (!ssdGet(self->settings, name, &val))
        return;

    ControlMsg* msg = controlMsgCreate(_S"FeatureSettings");
    cfieldSet(msg, _S"feature", string, self->name);
    sendSettingVal(msg, name, &val);
    cclientQueue(client, msg);
    stvarDestroy(&val);
}

void SubspaceFeature_sendAllSettings(_In_ SubspaceFeature* self, ControlClient* client)
{
    ControlMsg* msg = NULL;
    ssdLockedTransaction(self->settings)
    {
        int count = ssdCount(self->settings, NULL, false);
        if (count < 1)
            break;

        msg = controlMsgCreate(_S"FeatureSettings");

        cfieldSet(msg, _S"feature", string, self->name);
        foreach (ssd, iter, idx, name, val, self->settings) {
            sendSettingVal(msg, name, val);
        }
    }

    if (msg)
        cclientQueue(client, msg);
}

void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self)
{
    return;
}

void SubspaceFeature_sendSettingCur(_In_ SubspaceFeature* self, _In_opt_ strref name)
{
    GameInst* cur = subspaceGame(self->ss);
    if (!cur)
        return;

    ControlClient* client = objAcquireFromWeak(ControlClient, cur->client);
    if (client) {
        featureSendSetting(self, client, name);
        objRelease(&client);
    }

    objRelease(&cur);
}

bool SubspaceFeature_isEnabled(_In_ SubspaceFeature* self)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = self->enabled && self->available;
    }
    return ret;
}

bool SubspaceFeature_isAvailable(_In_ SubspaceFeature* self)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = self->available;
    }
    return ret;
}

bool SubspaceFeature_isLocked(_In_ SubspaceFeature* self)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = self->locked;
    }
    return ret;
}

// Autogen begins -----
// clang-format off
#include "feature/feature.auto.inc"
// clang-format on
// Autogen ends -------
