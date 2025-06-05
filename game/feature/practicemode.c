#include "feature/feature.h"
#include "patch/patchlist.h"

// ---- Patching ----------------

static bool practiceMode_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    return enabled;
}

Patch* PracticeMode_patches[] = { 0 };

SubspaceFeature PracticeMode_feature = { .name            = "PracticeMode",
                                         .enable          = practiceMode_Enable,
                                         .requiredPatches = PracticeMode_patches,
                                         .requiredSymbols = { 0 } };
