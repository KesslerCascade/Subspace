#include "control/controlclient.h"
#include "ftl/capp.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

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

void subspace_CApp_OnLoop_post(CApp* self) {}

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
                        0 }
};
