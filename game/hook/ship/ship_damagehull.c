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

// ---- Hooks ----------------

static int pre_currentHull;

int Ship_DamageHull_pre(Ship* self, int amount)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerCS         = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerCS ? CompleteShip_shipManager(playerCS) : NULL;
        Ship* playerShip = playerShipManager ? ShipManager_ship(playerShipManager) : NULL;
        if (amount != 0 && self == playerShip) {
            // This SHOULD only ever be called on the player's ship, but check anyway just in case
            // something crazy happens.
            pre_currentHull = Ship_hullIntegrity(self);
        }
    }

    return 1;
}

int Ship_DamageHull_post(int ret, Ship* self, int amount)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerCS         = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerCS ? CompleteShip_shipManager(playerCS) : NULL;
        Ship* playerShip = playerShipManager ? ShipManager_ship(playerShipManager) : NULL;
        const char* src  = gc.curDamageSource ? gc.curDamageSource : "Other";

        if (amount != 0 && self == playerShip) {
            int deltaHull = pre_currentHull - Ship_hullIntegrity(self);
            runLogSend(&Log_HullDamage, src, deltaHull);
        }
    }

    return ret;
}

// ---- Patch -------------pr---

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, Ship_DamageHull, Ship_DamageHull_pre, Ship_DamageHull_post);
}

Patch patch_Ship_DamageHull = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(Ship_DamageHull),
                        &SYM(Ship_hullIntegrity_offset),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(ShipManager_ship_offset),
                        0 }
};
