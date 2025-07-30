#include "ftl/stdlib.h"

#include "ftl/blueprintmanager.h"
#include "ftl/capp.h"
#include "ftl/scorekeeper.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
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
    &patch_WorldManager_OnLoop,
    &patch_StarMap_GenerateMap,
    0
};

SubspaceFeature Base_feature = {
    .name            = "Base",
    .requiredPatches = Base_patches,
    .requiredSymbols = { &SYM(crt_malloc),
                        &SYM(crt_free),
                        &SYM(crt_realloc),
                        &SYM(crt_rand),
                        &SYM(crt_srand),
                        &SYM(ShipBlueprint_blueprintName_offset),
                        &SYM(ShipBlueprint_name_offset),
                        &SYM(StarMap_sectorMapSeed_offset),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_starMap_worldLevel_offset),
                        &SYM(ScoreKeeper_Keeper),
                        0 }
};
