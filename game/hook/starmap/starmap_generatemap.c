#include "control/controlclient.h"
#include "control/runlog.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

Location*
subspace_StarMap_GenerateMap_post(Location* ret, StarMap* self, bool bTutorial, bool useOldSeed)
{
    Sector* cur = self ? StarMap_currentSector(self) : NULL;
    gs.gameTime = 0;

    if (cur) {
        ControlMsg* msg = controlNewMsg("Sector", 4);
        controlMsgInt(msg, 0, "num", StarMap_worldLevel(self) + 1);
        controlMsgInt(msg, 1, "seed", StarMap_currentSectorSeed(self));
        controlMsgStr(msg, 2, "type", Sector_description_type(cur)->buf);
        controlMsgBool(msg, 3, "secret", StarMap_bSecretSector(self));
        msg->priority = 1;   // make sure this gets sent after NewGame
        controlClientQueue(msg);

        if (!gc.loadingGame)
            runLogSend(&Log_Sector,
                       Sector_description_type(cur)->buf,
                       StarMap_currentSectorSeed(self));
    }

    return ret;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, StarMap_GenerateMap, NULL, subspace_StarMap_GenerateMap_post);
}

Patch patch_StarMap_GenerateMap = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(StarMap_GenerateMap),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(StarMap_currentSector_offset),
                        &SYM(StarMap_currentSectorSeed_offset),
                        &SYM(StarMap_bSecretSector_offset),
                        &SYM(StarMap_worldLevel_offset),
                        &SYM(Sector_description_type_offset),
                        0 }
};
