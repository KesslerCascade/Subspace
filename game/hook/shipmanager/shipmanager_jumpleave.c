#include "feature/feature.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "control/runlog.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int ShipManager_JumpLeave_pre(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        runLogSend(&Log_Jump);
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_JumpLeave,
                        ShipManager_JumpLeave_pre,
                        NULL);
}

Patch patch_ShipManager_JumpLeave = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_JumpLeave), 0 }
};
