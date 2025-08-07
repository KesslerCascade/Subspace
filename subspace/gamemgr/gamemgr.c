// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "gamemgr/gamemgr.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "ui/subspaceui.h"
#include "version.h"

_objfactory_guaranteed GameMgr* GameMgr_create(Subspace* subspace)
{
    GameMgr* self;
    self = objInstCreate(GameMgr);

    self->ss = subspace;

    objInstInit(self);
    return self;
}

_objinit_guaranteed bool GameMgr_init(_In_ GameMgr* self)
{
    pcgAutoSeed(&self->pcg);
    self->pws = procStartWatchThread();
    // Autogen begins -----
    rwlockInit(&self->gmgrlock);
    htInit(&self->insts, uint32, object, 16);
    return true;
    // Autogen ends -------
}

void GameMgr_destroy(_In_ GameMgr* self)
{
    procStopWatchThread(self->pws);
    // Autogen begins -----
    rwlockDestroy(&self->gmgrlock);
    htDestroy(&self->insts);
    // Autogen ends -------
}

void GameMgr_reg(_In_ GameMgr* self, GameInst* inst)
{
    withWriteLock (&self->gmgrlock) {
        htInsert(&self->insts, uint32, inst->cookie, object, inst);
    }
}

void GameMgr_unreg(_In_ GameMgr* self, GameInst* inst)
{
    withWriteLock (&self->gmgrlock) {
        htRemove(&self->insts, uint32, inst->cookie);
    }
}

GameInst* GameMgr_acquireByCookie(_In_ GameMgr* self, uint32 cookie)
{
    GameInst* ret = NULL;
    withReadLock (&self->gmgrlock) {
        htFind(self->insts, uint32, cookie, object, &ret);
    }
    return ret;
}

uint32 GameMgr_genCookie(_In_ GameMgr* self)
{
    uint32 ret = 0;
    withReadLock (&self->gmgrlock) {
        for (;;) {
            ret = pcgRandom(&self->pcg);
            // verify it isn't already in use
            if (!htHasKey(self->insts, uint32, ret))
                break;
        }
    }
    return ret;
}

bool GameMgr_play(_In_ GameMgr* self, GameInst** out)
{
    Subspace* ss = self->ss;

    string exepath = 0;
    ssdStringOut(ss->settings, _S"ftl/exe", &exepath);
    GameInst* ninst = ginstCreate(self, exepath, LAUNCH_PLAY);
    strDestroy(&exepath);
    gmgrReg(self, ninst);
    bool ret = ginstLaunch(ninst);

    // failed to launch, don't track it in gamemgr
    if (!ret)
        gmgrUnreg(self, ninst);

    // if we're launching to play, set this as the focused instance
    if (ret) {
        subspaceSetGame(ss, ninst);
    }

    if (out)
        *out = objAcquire(ninst);
    objRelease(&ninst);
    return ret;
}

bool GameMgr_validate(_In_ GameMgr* self, _In_opt_ strref ftlexe, GameInst** out)
{
    Subspace* ss = self->ss;

    string exepath = 0;
    if (!strEmpty(ftlexe))
        strDup(&exepath, ftlexe);
    else
        ssdStringOut(ss->settings, _S"ftl/exe", &exepath);

    GameInst* ninst = ginstCreate(self, exepath, LAUNCH_VALIDATE);
    strDestroy(&exepath);

    gmgrReg(self, ninst);
    bool ret = ginstLaunch(ninst);

    if (ret) {
        ssuiNotify(ss->ui, _S"Validate_Start", stvar(object, ninst));
    } else {
        // failed to launch, don't track it in gamemgr
        gmgrUnreg(self, ninst);
    }

    if (out)
        *out = objAcquire(ninst);
    objRelease(&ninst);
    return ret;
}

void GameMgr_checkStartupValidate(_In_ GameMgr* self)
{
    Subspace* ss     = self->ss;
    string lastcheck = 0, exepath = 0;
    ssdStringOut(ss->settings, _S"ftl/compatcheck", &lastcheck);
    ssdStringOut(ss->settings, _S"ftl/exe", &exepath);

    // if the current vrsion is different than the last time we checked, kick off a validation
    if (!strEmpty(exepath) && !strEq(lastcheck, (strref)subspace_version_str)) {
        gmgrValidate(self, NULL, NULL);
    }

    strDestroy(&lastcheck);
    strDestroy(&exepath);
}

// Autogen begins -----
#include "gamemgr/gamemgr.auto.inc"
// Autogen ends -------
