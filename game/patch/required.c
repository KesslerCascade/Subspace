#include "patchlist.h"

PatchSequence RequiredPatches = {
    &patch_CApp_OnExecute,           &patch_CApp_OnLoop,   &patch_ResourceControl_RenderLoadingBar,
    &patch_FileHelper_getUserFolder, &patch_MainMenu_Open, 0
};
