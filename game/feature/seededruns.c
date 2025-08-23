#include "seededruns.h"
#include "patch/patchlist.h"

// ---- Patching ----------------

Patch* SeededRuns_patches[] = { &patch_Misc_Random32, 0 };

SubspaceFeature SeededRuns_feature = { .name            = "SeededRuns",
                                       .requiredPatches = SeededRuns_patches,
                                       .requiredSymbols = { 0 } };
