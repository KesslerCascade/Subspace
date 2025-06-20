#include "control/runlog.h"
#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

DamageSource systemExplodedSrc;

bool ShipSystem_GetExploded_post(bool ret, ShipSystem *self)
{
    if (RunTracker_feature.enabled) {
        // This function is a one-shot; if it return true it also resets the flag in the system structure.
        // Because we can't do pre/post on it we have to reset the source in ShipManager::DamageHull
        if (ret)
            damageSourceSet(&systemExplodedSrc, "SystemDestroyed");
    }

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipSystem_GetExploded,
                        NULL,
                        ShipSystem_GetExploded_post);
}

Patch patch_ShipSystem_GetExploded = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipSystem_GetExploded),
                        0 }
};
