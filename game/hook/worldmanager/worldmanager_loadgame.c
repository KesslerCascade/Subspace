#include "control/controlclient.h"
#include "ftl/blueprintmanager.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/globals.h"
#include "ftl/scorekeeper.h"
#include "ftl/shipmanager.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "proto.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_WorldManager_LoadGame_post(WorldManager* self, basic_string* file)
{
    CompleteShip* playerShip = WorldManager_playerShip(self);
    StarMap* map             = WorldManager_starMap(self);
    ShipManager* smgr        = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
    ShipBlueprint* sbp       = smgr ? ShipManager_myBlueprint(smgr) : NULL;
    basic_string* shipType   = sbp ? ShipBlueprint_blueprintName(sbp) : NULL;
    TextString* shipName     = sbp ? ShipBlueprint_name(sbp) : NULL;
    int seed                 = map ? StarMap_sectorMapSeed(map) : 0;
    StatTracker* stats       = ScoreKeeper_stats(SKeeper);

    if (shipType && shipName && seed) {
        ControlMsg* msg = controlNewMsg("LoadGame", 5);
        controlMsgStr(msg, 0, "ship", shipType->buf);
        controlMsgStr(msg, 1, "name", shipName->data.buf);
        controlMsgInt(msg, 2, "seed", seed);
        controlMsgInt(msg, 3, "difficulty", g_Settings_difficulty);
        controlMsgInt(msg, 4, "beacons", stats[1].current);
        controlClientQueue(msg);

        msg = controlNewMsg("GameState", 1);
        controlMsgInt(msg, 0, "state", GAME_RUN);
        controlClientQueue(msg);
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, WorldManager_LoadGame, NULL, subspace_WorldManager_LoadGame_post);
}

Patch patch_WorldManager_LoadGame = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_LoadGame),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(StarMap_sectorMapSeed_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(ShipBlueprint_blueprintName_offset),
                        &SYM(ShipBlueprint_name_offset),
                        &SYM(Settings_difficulty),
                        &SYM(ScoreKeeper_Keeper),
                        0 }
};
