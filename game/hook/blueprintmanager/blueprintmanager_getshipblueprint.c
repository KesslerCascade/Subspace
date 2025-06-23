#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/blueprintmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

int BlueprintManager_GetShipBlueprint_pre(BlueprintManager* self, basic_string* name, int sector)
{
    if (RunTracker_feature.enabled && gc.loadingBossShip) {
        // The boss needs special handling; easiest way to get which version is being loaded is to
        // simply catch it as it's loaded
        if (name && name->len > 0) {
            runLogSend(&Log_Ship, name->buf);
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        BlueprintManager_GetShipBlueprint,
                        BlueprintManager_GetShipBlueprint_pre,
                        NULL);
}

Patch patch_BlueprintManager_GetShipBlueprint = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(BlueprintManager_GetShipBlueprint), 0 }
};
