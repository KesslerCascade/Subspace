#include "control/runlog.h"
#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

static DamageSource areasrc;

bool ShipManager_DamageArea_pre(ShipManager* self, Pointf location, Damage damage, bool forceHit)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerShip       = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        if (damage.iDamage > 0 && self == playerShipManager) {
            // Some special-case hacks.
            // These are less than ideal because they may not be accurate if there are mods in
            // place, but for now is the best we can do because there isn't a way to get the actual
            // projectile here. Maybe in the future when we have better hooks into
            // Projectile::CollisionCheck something better can be done there.
            if (damage.selfId == -1 && damage.iDamage == 3 && damage.iShieldPiercing == 5 &&
                damage.breachChance == 10 && damage.fireChance == 0)
                damageSourceSet(&areasrc, "ASB");
            if (damage.iDamage == 1 && damage.iShieldPiercing == 0 && damage.fireChance == 2 &&
                damage.breachChance == 1)
                damageSourceSet(&areasrc, "Asteroid");
            else if (damage.iShieldPiercing >= 5)
                damageSourceSet(&areasrc, "Missile");
            else
                damageSourceSet(&areasrc, "Projectile");   // something else
        }
    }

    return 1;
}

bool ShipManager_DamageArea_post(bool ret, ShipManager* self, Pointf location, Damage damage,
                                 bool forceHit)
{
    damageSourceFinish(&areasrc);
    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_DamageArea,
                        ShipManager_DamageArea_pre,
                        ShipManager_DamageArea_post);
}

Patch patch_ShipManager_DamageArea = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_DamageArea),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        0 }
};
