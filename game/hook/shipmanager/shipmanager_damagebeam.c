#include "control/runlog.h"
#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

static DamageSource beamsrc;

// ---- Hooks ----------------

bool ShipManager_DamageBeam_pre(ShipManager* self, Pointf current, Pointf last, Damage damage)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerShip       = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        if (damage.iDamage > 0 && self == playerShipManager) {
            damageSourceSet(&beamsrc,"Beam");
        }
    }

    return 1;
}

bool ShipManager_DamageBeam_post(bool ret, ShipManager* self, Pointf current, Pointf last,
                                 Damage damage)
{
    damageSourceFinish(&beamsrc);
    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, ShipManager_DamageBeam, ShipManager_DamageBeam_pre, NULL);
}

Patch patch_ShipManager_DamageBeam = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_DamageBeam),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        0 }
};
