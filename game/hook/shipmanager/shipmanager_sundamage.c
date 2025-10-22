#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

static EventSource flaresrc;

// ---- Hooks ----------------

int ShipManager_SunDamage_pre(ShipManager* self)
{
    if (RunTracker_feature.enabled) {
        eventSourceSet(Damage, &flaresrc, "SolarFlare");
    }

    return 1;
}

void ShipManager_SunDamage_post(ShipManager* self)
{
    eventSourceFinish(Damage, &flaresrc);
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
