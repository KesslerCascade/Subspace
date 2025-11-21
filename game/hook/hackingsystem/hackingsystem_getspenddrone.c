#include "feature/runtracker.h"
#include "ftl/hackingsystem.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"

EventSource hackingDroneSrc;

int subspace_HackingSystem_GetSpendDrone_post(int ret, HackingSystem* self)
{
    if (ret) {
        // only if we're using a hacking drone right now
        if (RunTracker_feature.enabled)
            resourceScan();

        eventSourceSet(Resource, &hackingDroneSrc, _S"Hacking");
    }

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        HackingSystem_GetSpendDrone,
                        NULL,
                        subspace_HackingSystem_GetSpendDrone_post);
}

Patch patch_HackingSystem_GetSpendDrone = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(HackingSystem_GetSpendDrone), 0 }
};
