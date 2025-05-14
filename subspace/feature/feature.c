// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "feature.h"
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

            ControlMsg* emsg = controlNewMsg("EnableFeature", 2);
            controlMsgStr(emsg, 0, "feature", self->name);
            controlMsgBool(emsg, 1, "enabled", enabled);
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

    // update entire UI when feature state changes
    subspaceUpdateUI(self->ss);
}

static bool sendSettingVal(ControlMsg* msg, int field, const char* fname, stvar* val)
{
    switch (stGetId(val->type)) {
    case STypeId_bool:
        controlMsgBool(msg, field, fname, val->data.st_bool);
        return true;
    case STypeId_int64:
        // parsed from json... have to figure out whether it's signed or unsigned
        if (val->data.st_int64 < 0)
            controlMsgInt(msg, field, fname, (int32)val->data.st_int64);
        else if (val->data.st_int64 > 0x7fffffffULL)
            controlMsgUInt(msg, field, fname, (uint32)val->data.st_int64);
        else
            controlMsgInt(msg, field, fname, (int32)val->data.st_int64);

        return true;
    case STypeId_int32:
        controlMsgInt(msg, field, fname, val->data.st_int32);
        return true;
    case STypeId_uint32:
        controlMsgUInt(msg, field, fname, val->data.st_uint32);
        return true;
    case STypeId_float32:
        controlMsgFloat32(msg, field, fname, val->data.st_float32);
        return true;
    case STypeId_float64:
        controlMsgFloat64(msg, field, fname, val->data.st_float64);
        return true;
    case STypeId_string:
        controlMsgStr(msg, field, fname, val->data.st_string);
        return true;
    }

    return false;
}

void SubspaceFeature_sendSetting(_In_ SubspaceFeature* self, ControlClient* client,
                                 _In_opt_ strref name)
{
    stvar val;

    if (!ssdGet(self->settings, name, &val))
        return;

    ControlMsg* msg = controlNewMsg("FeatureSettings", 2);
    controlMsgStr(msg, 0, "feature", self->name);
    sendSettingVal(msg, 1, strC(name), &val);
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

        msg = controlNewMsg("FeatureSettings", count + 1);

        int n = 1;
        controlMsgStr(msg, 0, "feature", self->name);
        foreach (ssd, iter, idx, name, val, self->settings) {
            sendSettingVal(msg, n++, strC(name), val);
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
    GameInst* cur = objAcquireFromWeak(GameInst, self->ss->curinst);
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

// Autogen begins -----
#include "feature.auto.inc"
// Autogen ends -------
