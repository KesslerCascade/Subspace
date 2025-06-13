#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/scorekeeper.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

#define NUM_STATS 4
static int lastStats[NUM_STATS];

// ---- Hooks ----------------

int subspace_CApp_OnLoop_pre(CApp* self)
{
    controlClientProcess();

    if (gs.sendSectorInfo) {
        gs.sendSectorInfo   = false;
        WorldManager* world = CApp_world(self);
        StarMap* map        = world ? WorldManager_starMap(world) : NULL;
        Sector* cur         = map ? StarMap_currentSector(map) : NULL;

        if (cur) {
            ControlMsg* msg = controlNewMsg("Sector", 4);
            controlMsgInt(msg, 0, "num", StarMap_worldLevel(map) + 1);
            controlMsgInt(msg, 1, "seed", StarMap_currentSectorSeed(map));
            controlMsgStr(msg, 2, "type", Sector_description_type(cur)->buf);
            controlMsgBool(msg, 3, "secret", Starmap_bSecretSector(map));
            controlClientQueue(msg);
        }
    }

    return 1;   // we do want to execute the original CApp::OnLoop
}

void subspace_CApp_OnLoop_post(CApp* self)
{
    StatTracker* stats = ScoreKeeper_stats(SKeeper);
    int statschanged   = 0;

    // we actually have hooks for the stat update functions, but it's super cheap to simply check
    // them each loop anyway, since for stat tracking we don't care about WHY it changed

    for (int i = 0; i < NUM_STATS; i++) {
        if (stats[i].current != lastStats[i])
            statschanged++;
    }

    if (gs.sendAllStats)
        statschanged = NUM_STATS;

    if (statschanged) {
        ControlMsg* msg = controlNewMsg("Stats", statschanged);

        int f = 0;
        if (gs.sendAllStats || stats[0].current != lastStats[0])
            controlMsgInt(msg, f++, "ships", stats[0].current);
        if (gs.sendAllStats || stats[1].current != lastStats[1])
            controlMsgInt(msg, f++, "beacons", stats[1].current);
        if (gs.sendAllStats || stats[2].current != lastStats[2])
            controlMsgInt(msg, f++, "scrap", stats[2].current);
        if (gs.sendAllStats || stats[3].current != lastStats[3])
            controlMsgInt(msg, f++, "crew", stats[3].current);
        controlClientQueue(msg);

        for (int i = 0; i < NUM_STATS; i++) {
            lastStats[i] = stats[i].current;
        }
    }

    gs.sendAllStats = false;

    if (RunTracker_feature.enabled)
        gc.curScrapSource = NULL;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnLoop, subspace_CApp_OnLoop_pre, subspace_CApp_OnLoop_post);
}

Patch patch_CApp_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CApp_OnLoop),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(StarMap_currentSector_offset),
                        &SYM(StarMap_currentSectorSeed_offset),
                        &SYM(StarMap_bSecretSector_offset),
                        &SYM(StarMap_worldLevel_offset),
                        &SYM(Sector_description_type_offset),
                        &SYM(ScoreKeeper_Keeper),
                        0 }
};
