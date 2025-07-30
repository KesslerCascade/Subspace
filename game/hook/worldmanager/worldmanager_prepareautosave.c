#include "control/controlclient.h"
#include "feature/savemanager.h"
#include "ftl/worldmanager.h"
#include "proto.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_WorldManager_PrepareAutoSave_post(WorldManager* self)
{
    if (SaveManager_feature.enabled && gs.autoSaveNext) {
        gs.autoSaveNext = false;
        saveManagerAutoSave(self);
    }

    return;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        WorldManager_PrepareAutoSave,
                        NULL,
                        subspace_WorldManager_PrepareAutoSave_post);
}

Patch patch_WorldManager_PrepareAutoSave = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WorldManager_PrepareAutoSave), 0 }
};
