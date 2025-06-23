#include "runtracker.h"
#include "feature/feature.h"
#include "patch/patchlist.h"

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
    0
};

SubspaceFeature RunTracker_feature = { .name            = "RunTracker",
                                       .enable          = runTracker_Enable,
                                       .requiredPatches = RunTracker_patches,
                                       .requiredSymbols = { 0 } };
