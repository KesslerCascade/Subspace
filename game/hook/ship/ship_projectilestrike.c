#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/ship.h"
#include "ftl/shipmanager.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

#include "minicrt.h"

// ---- Hooks ----------------

static int pre_currentHull;

int Ship_ProjectileStrike_pre(Ship* self, int roomId, float damage)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerCS         = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerCS ? CompleteShip_shipManager(playerCS) : NULL;
        Ship* playerShip = playerShipManager ? ShipManager_ship(playerShipManager) : NULL;
        if (damage > 0 && self == playerShip) {
            // This SHOULD only ever be called on the player's ship, but check anyway just in case
            // something crazy happens.
            pre_currentHull = MAX(Ship_hullIntegrity(self), 0);
        }
    }

    return 1;
}

void Ship_ProjectileStrike_post(Ship* self, int roomId, float damage)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerCS         = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerCS ? CompleteShip_shipManager(playerCS) : NULL;
        Ship* playerShip = playerShipManager ? ShipManager_ship(playerShipManager) : NULL;
        const char* src  = gc.curDamageSource ? gc.curDamageSource : "Projectile";

        if (damage > 0 && self == playerShip) {
            int deltaHull = pre_currentHull - MAX(Ship_hullIntegrity(self), 0);
            if (deltaHull != 0)
                runLogSend(&Log_HullDamage, src, deltaHull);
        }
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        Ship_ProjectileStrike,
                        Ship_ProjectileStrike_pre,
                        Ship_ProjectileStrike_post);
}

Patch patch_Ship_ProjectileStrike = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(Ship_ProjectileStrike),
                        &SYM(Ship_hullIntegrity_offset),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(ShipManager_ship_offset),
                        0 }
};
