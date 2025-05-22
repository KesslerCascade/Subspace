#include "ftl/blueprintmanager.h"
#include "patch/patchlist.h"
#include "feature.h"

// ---- Patching ----------------

Patch* Base_patches[] = {
    &patch_CApp_OnExecute,           &patch_CApp_OnLoop,   &patch_ResourceControl_RenderLoadingBar,
    &patch_FileHelper_getUserFolder, &patch_MainMenu_Open, 0
};

SubspaceFeature Base_feature = {
    .name            = "Base",
    .requiredPatches = Base_patches,
    .requiredSymbols = { &SYM(ShipBlueprint_blueprintName_offset),
                        &SYM(ShipBlueprint_name_offset),
                        0 }
};
