#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/shipmanager.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static const char* oldDamageSource;

// ---- Hooks ----------------

static int pre_currentScrap;

int ShipManager_ModifyScrapCount_pre(ShipManager* self, int amount, bool income)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerShip       = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        if (amount != 0 && self == playerShipManager) {
            // This SHOULD only ever be called on the player's ship, but check anyway just in case
            // something crazy happens.
            pre_currentScrap = ShipManager_currentScrap(self);
        }

        // for hull repair
        oldDamageSource    = gc.curDamageSource;
        gc.curDamageSource = "RepairArm";
    }

    return 1;
}

void ShipManager_ModifyScrapCount_post(ShipManager* self, int amount, bool income)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerShip       = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        const char* src                = gc.curScrapSource ? gc.curScrapSource : "Other";

        if (income)
            src = "Event";

        if (amount != 0 && self == playerShipManager) {
            int deltaScrap = ShipManager_currentScrap(self) - pre_currentScrap;
            runLogSend(&Log_Scrap, src, deltaScrap, amount);
        }

        gc.curDamageSource = oldDamageSource;
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipManager_ModifyScrapCount,
                        ShipManager_ModifyScrapCount_pre,
                        ShipManager_ModifyScrapCount_post);
}

Patch patch_ShipManager_ModifyScrapCount = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipManager_ModifyScrapCount),
                        &SYM(ShipManager_currentScrap_offset),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        0 }
};
