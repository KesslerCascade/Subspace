#include "control/controlclient.h"
#include "feature/tweaks.h"
#include "ftl/worldmanager.h"
#include "proto.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_WorldManager_SaveGame_pre(WorldManager* self)
{
    if (Tweaks_feature.enabled)
        tweaksPrepareSave();

    return 1;
}

void subspace_WorldManager_SaveGame_post(WorldManager* self)
{
    if (Tweaks_feature.enabled)
        tweaksFinalizeSave();

    return;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_SaveGame,
                        subspace_WorldManager_SaveGame_pre,
                        subspace_WorldManager_SaveGame_post);
}

Patch patch_WorldManager_SaveGame = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_SaveGame), 0 }
};
