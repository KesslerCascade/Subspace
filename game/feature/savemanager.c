#include "feature/feature.h"
#include "patch/patchlist.h"

// ---- Patching ----------------

static bool saveManager_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    return enabled;
}

Patch* SaveManager_patches[] = { 0 };

SubspaceFeature SaveManager_feature = { .name            = "SaveManager",
                                         .enable          = saveManager_Enable,
                                         .requiredPatches = SaveManager_patches,
                                         .requiredSymbols = { 0 } };
