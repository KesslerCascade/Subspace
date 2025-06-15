#include "feature/feature.h"
#include "patch/patchlist.h"

// ---- Patching ----------------

static bool runTracker_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    return enabled;
}

Patch* RunTracker_patches[] = { &patch_ShipManager_ModifyScrapCount,
                                &patch_GameOver_SetVictory,
                                0 };

SubspaceFeature RunTracker_feature = { .name            = "RunTracker",
                                       .enable          = runTracker_Enable,
                                       .requiredPatches = RunTracker_patches,
                                       .requiredSymbols = { 0 } };
