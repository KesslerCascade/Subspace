#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/weaponsystem.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource launchsrc;

// ---- Hooks ----------------

int subspace_WeaponSystem_OnLoop_pre(WeaponSystem* self)
{
    // only care about player ship
    if (ShipSystem_iShipId(self) != 0)
        return 1;

    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceSet(Resource, &launchsrc, _S"Launch");

    return 1;
}

void subspace_WeaponSystem_OnLoop_post(WeaponSystem* self)
{
    // only care about player ship
    if (ShipSystem_iShipId(self) != 0)
        return;

    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceFinish(Resource, &launchsrc);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WeaponSystem_OnLoop,
                        subspace_WeaponSystem_OnLoop_pre,
                        subspace_WeaponSystem_OnLoop_post);
}

Patch patch_WeaponSystem_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WeaponSystem_OnLoop), 0 }
};
