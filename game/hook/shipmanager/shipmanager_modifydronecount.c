#include "feature/runtracker.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"

int subspace_ShipManager_ModifyDroneCount_post(ShipManager* ship, int amount)
{
        if (RunTracker_feature.enabled)
            resourceScan();

        eventSourceFinish(Resource, &hackingDroneSrc);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_ModifyDroneCount,
                        NULL,
                        subspace_ShipManager_ModifyDroneCount_post);
}

Patch patch_ShipManager_ModifyDroneCount = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_ModifyDroneCount), 0 }
};
