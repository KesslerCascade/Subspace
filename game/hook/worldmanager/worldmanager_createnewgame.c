#include "control/controlclient.h"
#include "control/runlog.h"
#include "feature/runtracker.h"
#include "ftl/blueprintmanager.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/globals.h"
#include "ftl/shipmanager.h"
#include "ftl/starmap.h"
#include "ftl/tutorialmanager.h"
#include "ftl/worldmanager.h"
#include "inventory/inventory.h"
#include "inventory/resources.h"
#include "proto.h"
#include "subspacegame.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_WorldManager_CreateNewGame_post(WorldManager* self)
{
    if (!TutorialManager_Running(Tutorial)) {
        CompleteShip* playerShip = WorldManager_playerShip(self);
        StarMap* map             = WorldManager_starMap(self);
        ShipManager* smgr        = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
        ShipBlueprint* sbp       = smgr ? ShipManager_myBlueprint(smgr) : NULL;
        basic_string* shipType   = sbp ? ShipBlueprint_blueprintName(sbp) : NULL;
        TextString* shipName     = sbp ? ShipBlueprint_name(sbp) : NULL;
        int seed                 = map ? StarMap_sectorMapSeed(map) : 0;

        if (shipType && shipName && seed) {
            ControlMsg* msg = controlMsgCreate(_S"NewGame");
            cfieldSet(msg, _S"ship", strref, (strref)shipType->buf);
            cfieldSet(msg, _S"name", strref, (strref)shipName->data.buf);
            cfieldSet(msg, _S"seed", int32, seed);
            cfieldSet(msg, _S"difficulty", int32, g_Settings_difficulty);
            controlClientQueue(msg);

            msg = controlMsgCreate(_S"GameState");
            cfieldSet(msg, _S"state", int32, GAME_RUN);
            controlClientQueue(msg);

            runLogSend(&Log_Start, shipType->buf, shipName->data.buf, seed, g_Settings_difficulty);
        }
    } else {
        ControlMsg* msg = controlMsgCreate(_S"GameState");
        cfieldSet(msg, _S"state", int32, GAME_TUTORIAL);
        controlClientQueue(msg);
    }

    // save initial ship inventory
    if (RunTracker_feature.enabled) {
        EventSource origsrc = { 0 };
        eventSourceSet(Inv, &origsrc, _S"Starting");
        invReset();
        invScan();
        eventSourceFinish(Inv, &origsrc);

        eventSourceSet(Resource, &origsrc, _S"Starting");
        resourceReset();
        resourceScan();
        eventSourceFinish(Resource, &origsrc);
    }

    gs.sendAllStats = true;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_CreateNewGame,
                        NULL,
                        subspace_WorldManager_CreateNewGame_post);
}

Patch patch_WorldManager_CreateNewGame = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_CreateNewGame),
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(StarMap_sectorMapSeed_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(ShipBlueprint_blueprintName_offset),
                        &SYM(ShipBlueprint_name_offset),
                        &SYM(Settings_difficulty),
                        &SYM(TutorialManager_Running),
                        0 }
};
