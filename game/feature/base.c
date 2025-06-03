#include "ftl/blueprintmanager.h"
#include "ftl/starmap.h"
#include "patch/patchlist.h"
#include "feature.h"

// ---- Patching ----------------

Patch* Base_patches[] = {
    &patch_CApp_OnExecute,
    &patch_CApp_OnLoop,
    &patch_ResourceControl_RenderLoadingBar,
    &patch_FileHelper_getUserFolder,
    &patch_MainMenu_Open,
    &patch_WorldManager_CreateNewGame,
    &patch_WorldManager_LoadGame,
    0
};

SubspaceFeature Base_feature = {
    .name            = "Base",
    .requiredPatches = Base_patches,
    .requiredSymbols = { &SYM(ShipBlueprint_blueprintName_offset),
                        &SYM(ShipBlueprint_name_offset),
                        &SYM(StarMap_sectorMapSeed_offset),
                        0 }
};
