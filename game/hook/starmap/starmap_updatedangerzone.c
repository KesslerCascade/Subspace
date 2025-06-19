#include "feature/feature.h"
#include "ftl/starmap.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "control/runlog.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_StarMap_UpdateDangerZone_pre(StarMap *self)
{
    if (RunTracker_feature.enabled) {
        if (!gs.waitInProgress && StarMap_waiting_running(self)) {
            runLogSend(&Log_Wait);
            gs.waitInProgress = true;
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, StarMap_UpdateDangerZone, subspace_StarMap_UpdateDangerZone_pre, NULL);
}

Patch patch_StarMap_UpdateDangerZone = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(StarMap_UpdateDangerZone), 
        &SYM(StarMap_waiting_running_offset),0 }
};
