#include "control/runlog.h"
#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int ShipManager_DamageHull_post(int ret, ShipManager* ship, int dmg, bool force)
{
    // this is set in ShipSystem::GetExploded, but DamageHull is called right after
    damageSourceFinish(&systemExplodedSrc);

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_DamageHull,
                        NULL,
                        ShipManager_DamageHull_post);
}

Patch patch_ShipManager_DamageHull = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_DamageHull),
                        0 }
};
