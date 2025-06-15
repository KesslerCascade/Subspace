#include "control/runlog.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/gameover.h"
#include "ftl/shipmanager.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_GameOver_SetVictory_pre(GameOver* self, bool victory, basic_string* shipName,
                                     void* crew)
{
    if (RunTracker_feature.enabled) {
        if (victory) {
            runLogSend(&Log_Victory);
        } else {
            const char* reason  = "Other";
            WorldManager* world = CApp_world(theApp);
            CompleteShip* ship  = world ? WorldManager_playerShip(world) : NULL;
            ShipManager* shipm  = ship ? CompleteShip_shipManager(ship) : NULL;
            StarMap* map        = world ? WorldManager_starMap(world) : NULL;

            if (shipm && map) {
                // check various game over conditions
                if (ShipManager_bDestroyed(shipm)) {
                    reason = "Destroyed";
                } else if (CompleteShip_DeadCrew(ship)) {
                    reason = "CrewDead";
                } else if (StarMap_CheckGameOver(map)) {
                    reason = "BaseDestroyed";
                }

                runLogSend(&Log_Defeat, reason);
            }
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, GameOver_SetVictory, subspace_GameOver_SetVictory_pre, NULL);
}

Patch patch_GameOver_SetVictory = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(GameOver_SetVictory),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(CompleteShip_DeadCrew),
                        &SYM(ShipManager_bDestroyed_offset),
                        &SYM(StarMap_CheckGameOver),
                        0 }
};
