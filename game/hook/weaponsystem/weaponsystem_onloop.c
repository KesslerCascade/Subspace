#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/weaponsystem.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource usesrc;

// ---- Hooks ----------------

int subspace_WeaponSystem_OnLoop_pre(WeaponSystem* self)
{
    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceSet(Resource, &usesrc, _S"Use");

    return 1;
}

void subspace_WeaponSystem_OnLoop_post(WeaponSystem* self)
{
    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceFinish(Resource, &usesrc);
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
