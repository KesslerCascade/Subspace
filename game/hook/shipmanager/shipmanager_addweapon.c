#include "feature/feature.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "control/runlog.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int ShipManager_AddWeapon_pre(ShipManager* self, WeaponBlueprint* weapon, int slot)
{
    return 1;
}

int ShipManager_AddWeapon_post(int ret, ShipManager* self, WeaponBlueprint* weapon, int slot)
{
    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_AddWeapon,
                        ShipManager_AddWeapon_pre,
                        ShipManager_AddWeapon_post);
}

Patch patch_ShipManager_AddWeapon = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_AddWeapon), 0 }
};