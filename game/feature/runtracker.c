#include "runtracker.h"
#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/commandgui.h"
#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "ftl/equipment.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "ftl/projectilefactory.h"
#include "ftl/shipmanager.h"
#include "ftl/shipobject.h"
#include "ftl/weaponsystem.h"
#include "patch/patchlist.h"

static bool recording = false;

void _eventSourceSet(EventSource* es, string* cur, strref src)
{
    if (!es->set) {
        strDup(&es->prev, *cur);
        strDup(cur, src);
        es->set = true;
    }
}
void _eventSourceFinish(EventSource* es, string* cur)
{
    if (es->set) {
        strDup(cur, es->prev);
        strDestroy(&es->prev);
        es->set  = false;
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
    &patch_WorldManager_ModifyResources,
    0
};

SubspaceFeature RunTracker_feature = {
    .name            = _S"RunTracker",
    .enable          = runTracker_Enable,
    .requiredPatches = RunTracker_patches,
    .requiredSymbols = { &SYM(freetype_easy_printRightAlign),
                        &SYM(freetype_easy_measurePrintLines),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(ShipManager_weaponSystem_offset),
                        &SYM(WeaponSystem_weapons_offset),
                        &SYM(DroneSystem_drones_offset),
                        &SYM(ProjectileFactory_blueprint_offset),
                        &SYM(CommandGui_equipScreen_offset),
                        &SYM(Equipment_vEquipmentBoxes_offset),
                        &SYM(EquipmentBox_item_offset),
                        &SYM(Equipment_overcapacityBox_offset),
                        &SYM(Equipment_overAugBox_offset),
                        &SYM(Equipment_cargoId_offset),
                        &SYM(Drone_blueprint_offset),
                        &SYM(ShipObject_shipInfoList),
                        0 }
};
