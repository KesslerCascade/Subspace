#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

Location*
subspace_StarMap_GenerateMap_post(Location* ret, StarMap* self, bool bTutorial, bool useOldSeed)
{
    // this gets called during new game creation, but we'd rather the Sector message come AFTER, so
    // defer it until the next loop
    gs.sendSectorInfo = true;

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
    .requiredSymbols = { &SYM(StarMap_GenerateMap), 0 }
};
