#include "control/runlog.h"
#include "feature/runtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

static EventSource repairsrc;
static int pre_currentScrap;

// ---- Hooks ----------------

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
        eventSourceSet(Damage, &repairsrc, _S"RepairArm");
    }

    return 1;
}

void ShipManager_ModifyScrapCount_post(ShipManager* self, int amount, bool income)
{
    if (RunTracker_feature.enabled) {
        WorldManager* world            = CApp_world(theApp);
        CompleteShip* playerShip       = world ? WorldManager_playerShip(world) : NULL;
        ShipManager* playerShipManager = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        strref src                     = gc.curScrapSource ? gc.curScrapSource : _S"Other";

        if (income)
            src = _S"Event";

        if (amount != 0 && self == playerShipManager) {
            int deltaScrap = ShipManager_currentScrap(self) - pre_currentScrap;
            runLogSend(&Log_Scrap, src, deltaScrap, amount);
        }
    }

    eventSourceFinish(Damage, &repairsrc);
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
