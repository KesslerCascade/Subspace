// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "runtracker.h"
// clang-format on
// ==================== Auto-generated section ends ======================
#include "control/controlserver.h"
#include "feature/featureregistry.h"
#include "feature/savemanager/savemanager.h"
#include "gamemgr/gameinst.h"
#include "ui/subspaceui.h"
#include "runtrackerpage.h"

_objfactory_guaranteed RunTracker* RunTracker_create(Subspace* ss)
{
    RunTracker* self;
    self = objInstCreate(RunTracker);

    self->ss = ss;
    strDup(&self->name, _S"RunTracker");

    objInstInit(self);
    return self;
}

extern SettingsPage*
SubspaceFeature_createSettingsPage(_In_ SubspaceFeature* self, SubspaceUI* ui);   // parent
#define parent_createSettingsPage(ui) \
    SubspaceFeature_createSettingsPage((SubspaceFeature*)(self), ui)
SettingsPage* RunTracker_createSettingsPage(_In_ RunTracker* self, SubspaceUI* ui)
{
    return SettingsPage(runtrackerpageCreate(self, ui));
}

extern void SubspaceFeature_applyDefaultSettings(_In_ SubspaceFeature* self);   // parent
#define parent_applyDefaultSettings() SubspaceFeature_applyDefaultSettings((SubspaceFeature*)(self))
void RunTracker_applyDefaultSettings(_In_ RunTracker* self)
{
    return;
}

extern void SubspaceFeature_enable(_In_ SubspaceFeature* self, bool enabled);   // parent
#define parent_enable(enabled) SubspaceFeature_enable((SubspaceFeature*)(self), enabled)
void RunTracker_enable(_In_ RunTracker* self, bool enabled)
{
    if (!featureIsLocked(self)) {
        SaveManager* smfeat = fregGet(SaveManager, self->ss->freg);
        if (smfeat) {
            if (enabled) {
                withWriteLock (&smfeat->lock) {
                    smfeat->locked = false;
                }
            } else {
                featureEnable(smfeat, false);
                withWriteLock (&smfeat->lock) {
                    smfeat->locked = true;
                }
            }
        }

        parent_enable(enabled);
    }
}

bool RunTracker_isPaused(_In_ RunTracker* self)
{
    bool ret = false;
    withReadLock (&self->lock) {
        ret = self->paused;
    }
    return ret;
}

void RunTracker_pause(_In_ RunTracker* self, bool paused)
{
    withWriteLock (&self->lock) {
        self->paused = paused;
    }

    // update sidebar state
    ssuiUpdateMain(self->ss->ui, NULL);
}

void RunTracker_sendUpdate(_In_ RunTracker* self, bool recording)
{
    GameInst* inst        = subspaceGame(self->ss);
    ControlClient* client = inst ? objAcquireFromWeak(ControlClient, inst->client) : NULL;

    if (client) {
        ControlMsg* msg = controlNewMsg("RunTrackerUpd", 1);
        controlMsgBool(msg, 0, "recording", recording);
        cclientQueue(client, msg);
    }

    objRelease(&client);
    objRelease(&inst);
}

void RunTracker_updateLockState(_In_ RunTracker* self)
{
    bool locked    = false;
    GameInst* inst = subspaceGame(self->ss);
    // NOTE: this is only ever called with the GameInst lock already held
    if (inst && inst->state != GI_Menu) {
        // can't change this if the game isn't running or is at the menu
        locked = true;
    }

    bool oldlocked;
    withWriteLock (&self->lock) {
        oldlocked    = self->locked;
        self->locked = locked;
    }

    if (oldlocked != locked)
        ssuiUpdateSettings(self->ss->ui, _S"features");
    objRelease(&inst);
    return;
}

// Autogen begins -----
#include "runtracker.auto.inc"
// Autogen ends -------
