// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gameinst.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include <cx/format.h>
#include "control/controlclient.h"
#include "control/controlserver.h"
#include "feature/featureregistry.h"
#include "ui/subspaceui.h"
#include "gamemgr.h"
#include "process.h"

_objfactory_guaranteed GameInst*
GameInst_create(GameMgr* mgr, _In_opt_ strref exepath, LaunchMode mode)
{
    GameInst* self;
    self = objInstCreate(GameInst);

    self->mgr = objGetWeak(GameMgr, mgr);
    self->ss  = mgr->ss;
    strDup(&self->exepath, exepath);
    self->mode   = mode;
    self->cookie = gmgrGenCookie(mgr);

    objInstInit(self);
    return self;
}

_objfactory_guaranteed GameInst*
GameInst_createForClient(GameMgr* mgr, ControlClient* client, uint32 cookie)
{
    GameInst* self;
    self = objInstCreate(GameInst);

    self->mgr    = objGetWeak(GameMgr, mgr);
    self->ss     = mgr->ss;
    self->client = objGetWeak(ControlClient, client);
    self->cookie = cookie;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool GameInst_init(_In_ GameInst* self)
{
    // Autogen begins -----
    rwlockInit(&self->lock);
    htInit(&self->features, string, object, 16);
    return true;
    // Autogen ends -------
}

static void instCloseCb(ProcessHandle* h, uint32 pid, void* userdata)
{
    Weak(GameInst)* giw = (Weak(GameInst)*)userdata;
    GameInst* inst      = objAcquireFromWeak(GameInst, giw);

    if (inst) {
        Subspace* ss  = inst->ss;
        // unregister from game manager since it's not running anymore
        GameMgr* gmgr = objAcquireFromWeak(GameMgr, inst->mgr);
        if (gmgr) {
            gmgrUnreg(gmgr, inst);
            objRelease(&gmgr);
        }

        if (ss->curinst == atomicLoad(ptr, &inst->_weakref, Relaxed)) {
            objDestroyWeak(&ss->curinst);
            ssuiUpdateMain(ss->ui, NULL);
            ssuiUpdateMain(ss->ui, _S"gameinfo");
        }

        withWriteLock (&inst->lock) {
            procCloseHandle(&inst->process);
            inst->process = NULL;
            if (inst->state != GI_Failed && inst->state != GI_Validated)
                ginstSetStateLocked(inst, GI_Exited);
        }
        objRelease(&inst);
    }

    objDestroyWeak(&giw);
}

bool GameInst_launch(_In_ GameInst* self)
{
    bool ret = false;

    if (strEmpty(self->ss->gamepath))
        return false;

    GameMgr* mgr = objAcquireFromWeak(GameMgr, self->mgr);
    if (!mgr)
        return false;

    sa_string args;
    string tmp = 0;
    saInit(&args, string, 8);

    saPush(&args, string, _S"-port");
    strFromInt32(&tmp, self->ss->svr->port, 10);
    saPushC(&args, string, &tmp);

    saPush(&args, string, _S"-cookie");
    strFromInt32(&tmp, self->cookie >> 16, 16);
    saPushC(&args, string, &tmp);
    strFromInt32(&tmp, self->cookie & 0xffff, 16);
    saPushC(&args, string, &tmp);

    withWriteLock (&self->lock) {
        if (self->state == GI_Init) {
            self->process = procRun(self->ss->gamepath, args);
            if (self->process) {
                ret = true;
                ginstSetStateLocked(self, GI_Launching);
                procWatchAdd(mgr->pws, &self->process, instCloseCb, objGetWeak(GameInst, self));
            }
        }
    }

    strDestroy(&tmp);
    saDestroy(&args);
    objRelease(&mgr);

    return ret;
}

GameInstState GameInst_getState(_In_ GameInst* self)
{
    GameInstState ret;
    withReadLock (&self->lock) {
        ret = self->state;
    }

    return ret;
}

void GameInst_setState(_In_ GameInst* self, GameInstState state)
{
    withWriteLock (&self->lock) {
        ginstSetStateLocked(self, state);
    }
}

void GameInst_setStateLocked(_In_ GameInst* self, GameInstState state)
{
    if (self->state != state) {
        self->state = state;
        if ((void*)self->ss->curinst == atomicLoad(ptr, &self->_weakref, Relaxed)) {
            ssuiUpdateMain(self->ss->ui, NULL);
            ssuiUpdateMain(self->ss->ui, _S"gameinfo");
        }
    }
}

void GameInst_onGameReady(_In_ GameInst* self, ControlClient* client)
{
    Subspace* ss = self->ss;
    // game has signaled that it's ready

    // reconcile feature state with the connected client
    foreach (hashtable, hti, ss->freg->features) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(object, hti);

        // send settings for all features, whether they're enabled or not
        featureSendAllSettings(feat, client);

        ClientFeature* cfeat = NULL;
        if (htFind(self->features, string, feat->name, object, &cfeat)) {
            featureSetAvailable(feat, cfeat->available);

            if (featureIsEnabled(feat)) {
                ControlMsg* msg = controlNewMsg("EnableFeature", 2);
                controlMsgStr(msg, 0, "feature", feat->name);
                controlMsgBool(msg, 1, "enabled", true);
                cclientQueue(client, msg);
            }
            // cfeat->enabled will be set when the client sends a FeatureState update
            objRelease(&cfeat);
        } else {
            featureSetAvailable(feat, false);
        }

        // cache available state
        string epath = 0;
        strNConcat(&epath, _S"feature/", feat->name, _S"/available");
        ssdSet(self->ss->settings, epath, true, stvar(bool, feat->available));
        strDestroy(&epath);
    }

    // give it the clear to start
    ControlMsg* msg = controlNewMsg("ClearToStart", 0);
    cclientQueue(client, msg);

    // update UI because feature availabilty may have changed
    subspaceUpdateUI(ss);
}

void GameInst_destroy(_In_ GameInst* self)
{
    if (self->process)
        procCloseHandle(&self->process);
    // Autogen begins -----
    objDestroyWeak(&self->mgr);
    objDestroyWeak(&self->client);
    rwlockDestroy(&self->lock);
    strDestroy(&self->exepath);
    htDestroy(&self->features);
    // Autogen ends -------
}

// Autogen begins -----
#include "gameinst.auto.inc"
// Autogen ends -------
