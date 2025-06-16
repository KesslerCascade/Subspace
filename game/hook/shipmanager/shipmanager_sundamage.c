#include "feature/feature.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

static const char* oldDamageSource;

int ShipManager_SunDamage_pre(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        oldDamageSource    = gc.curDamageSource;
        gc.curDamageSource = "SolarFlare";
    }

    return 1;
}

void ShipManager_SunDamage_post(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        gc.curDamageSource = oldDamageSource;
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_SunDamage,
                        ShipManager_SunDamage_pre,
                        ShipManager_SunDamage_post);
}

Patch patch_ShipManager_SunDamage = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_SunDamage), 0 }
};
