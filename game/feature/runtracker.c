#include "runtracker.h"
#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "patch/patchlist.h"

static bool recording = false;

void damageSourceSet(DamageSource* ds, const char* src)
{
    if (!ds->set) {
        ds->prev           = gc.curDamageSource;
        ds->set            = true;
        gc.curDamageSource = src;
    }
}

void damageSourceFinish(DamageSource* ds)
{
    if (ds->set) {
        gc.curDamageSource = ds->prev;
        ds->prev           = NULL;
        ds->set            = false;
    }
}

void runTrackerHandleUpdate(bool r)
{
    recording = r;
}

bool runTrackerRenderWarning(void)
{
    // if we're recording, check if we're still connected
    if (!recording || controlClientConnected() || gs.renderingScreenshot)
        return false;

    GL_Color color = { 1, 0.2, 0.2, 1 };

    basic_string tmp;
    Pointf sz;
    float x = 1280 - 5;
    float y = 5;

    CSurface_GL_SetColor(color);
    basic_string_set(&tmp, "WARNING! Subspace is disconnected!");
    sz = easy_printRightAlign(1, x, y, &tmp);
    y  = sz.y;
    basic_string_destroy(&tmp);

    basic_string_set(&tmp, "Run tracker is NOT recording data.");
    easy_printRightAlign(1, x, y, &tmp);
    basic_string_destroy(&tmp);

    return true;
}

// ---- Patching ----------------

static bool runTracker_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    return enabled;
}

Patch* RunTracker_patches[] = {
    &patch_ShipManager_ModifyScrapCount,
    &patch_GameOver_SetVictory,
    &patch_Ship_DamageHull,
    &patch_Ship_ProjectileStrike,
    &patch_ShipManager_DamageArea,
    &patch_ShipManager_DamageBeam,
    &patch_ShipManager_DamageHull,
    &patch_ShipManager_SunDamage,
    &patch_ShipManager_JumpLeave,
    &patch_ShipSystem_GetExploded,
    &patch_StarMap_UpdateDangerZone,
    &patch_WorldManager_CreateLocation,
    &patch_WorldManager_UpdateLocation,
    &patch_WorldManager_CreateShip,
    &patch_BlueprintManager_GetShipBlueprint,
    &patch_CommandGui_GetCommand,
    &patch_FTLButton_OnRender,
    0
};

SubspaceFeature RunTracker_feature = {
    .name            = "RunTracker",
    .enable          = runTracker_Enable,
    .requiredPatches = RunTracker_patches,
    .requiredSymbols = { &SYM(freetype_easy_printRightAlign),
                        &SYM(freetype_easy_measurePrintLines),
                        &SYM(CSurface_GL_SetColor),
                        0 }
};
