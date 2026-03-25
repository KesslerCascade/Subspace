#include "control/runlog.h"
#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource jumpsrc;

// ---- Hooks ----------------

int ShipManager_JumpLeave_pre(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        runLogSend(&Log_Jump);
        resourceScan();
    }

    eventSourceSet(Resource, &jumpsrc, _S"Jump");

    return 1;
}

void ShipManager_JumpLeave_post(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceFinish(Resource, &jumpsrc);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_JumpLeave,
                        ShipManager_JumpLeave_pre,
                        ShipManager_JumpLeave_post);
}

Patch patch_ShipManager_JumpLeave = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_JumpLeave), 0 }
};
