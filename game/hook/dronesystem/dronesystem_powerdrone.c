#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/dronesystem.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource deploysrc;

// ---- Hooks ----------------

int subspace_DroneSystem_PowerDrone_pre(DroneSystem* self)
{
    // only care about player ship
    if (ShipSystem_iShipId(self) != 0)
        return 1;

    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceSet(Resource, &deploysrc, _S"DeployDrone");

    return 1;
}

int subspace_DroneSystem_PowerDrone_post(int ret, DroneSystem* self)
{
    // only care about player ship
    if (ShipSystem_iShipId(self) != 0)
        return ret;

    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceFinish(Resource, &deploysrc);

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        DroneSystem_PowerDrone,
                        subspace_DroneSystem_PowerDrone_pre,
                        subspace_DroneSystem_PowerDrone_post);
}

Patch patch_DroneSystem_PowerDrone = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(DroneSystem_PowerDrone), 0 }
};
