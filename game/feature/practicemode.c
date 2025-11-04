#include "ftl/stdlib.h"

#include "control/controlclient.h"
#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "ftl/mainmenu.h"
#include "ftl/powermanager.h"
#include "ftl/shipgraph.h"
#include "ftl/worldmanager.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"

void practiceSetSave(const char* fn)
{
    if (!PracticeMode_feature.enabled)
        return;

    if (gs.practiceSave)
        xa_free(gs.practiceSave);

    int l           = strlen(fn);
    gs.practiceSave = xa_malloc(l + 1);
    memcpy(gs.practiceSave, fn, l + 1);
}

void practiceLoad()
{
    if (!PracticeMode_feature.enabled || !gs.practiceSave)
        return;

    WorldManager* world = CApp_world(theApp);
    CommandGui* gui     = CApp_gui(theApp);
    MainMenu* menu      = CApp_menu(theApp);

    if (world && gui && menu) {
        // reset state before loading save
        // restart GUI if a ship exists, to make sure store window is closed while loading
        gs.practiceMode = true;

        WorldManager_ClearLocation(world);
        ShipGraph_Restart();
        PowerManager_RestartAll();
        if (WorldManager_playerShip(world)) {
            CommandGui_Restart(gui);
            CommandGui_OnCleanup(gui);
        }

        basic_string fname;
        basic_string_set(&fname, gs.practiceSave);
        WorldManager_LoadGame(world, &fname);
        basic_string_destroy(&fname);

        // without this the game stays at the menu after loading
        MainMenu_Close(menu);

        ControlMsg* msg = controlMsgCreate(_S"GameState");
        cfieldSet(msg, _S"state", int32, GAME_PRACTICE);
        controlClientQueue(msg);
    }
}

void practiceRenderNotice(void)
{
    // only show the notice when in practice mode
    if (!gs.practiceMode)
        return;

    GL_Color color = { 1, 0.51, 0.18, 1 };

    basic_string tmp;
    Pointf sz;
    float x = 633;
    float y = 8;

    CSurface_GL_SetColor(color);
    basic_string_set(&tmp, "Practice Mode");
    sz = easy_print(13, x, y, &tmp);
    y  = sz.y;
    basic_string_destroy(&tmp);
}

// ---- Patching ----------------

static bool practiceMode_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    return enabled;
}

static void practicemode_key_revert_cb(int key, int flags)
{
    if (PracticeMode_feature.enabled && gs.practiceMode)
        gs.practiceLoadSave = true;
}

static void practicemode_key_cheats_cb(int key, int flags)
{
    return;
}

static KeyBind PracticeMode_keybinds[] = {
    { .name = "practicemode_revert", .context = KB_CTX_GAME, .func = practicemode_key_revert_cb },
    { .name      = "practicemode_cheats",
     .context   = KB_CTX_GAME,
     .flags_exc = KB_JUMPING,
     .func      = practicemode_key_cheats_cb },
    { 0 }
};

Patch* PracticeMode_patches[] = {
    &patch_AchievementTracker_SetAchievement,
    &patch_CApp_OnLoop,
    &patch_CommandGui_RenderStatic,
    &patch_FileHelper_deleteAllSaveFiles,
    &patch_FileHelper_deleteFile,
    &patch_MainMenu_Open,
    &patch_ScoreKeeper_Various,
    &patch_WorldManager_SaveGame,
    0
};

SubspaceFeature PracticeMode_feature = {
    .name            = "PracticeMode",
    .enable          = practiceMode_Enable,
    .requiredPatches = PracticeMode_patches,
    .keybinds        = PracticeMode_keybinds,
    .requiredSymbols = { &SYM(WorldManager_LoadGame),
                        &SYM(WorldManager_StartGame),
                        &SYM(CApp_world_offset),
                        &SYM(CommandGui_OnCleanup),
                        &SYM(CommandGui_Restart),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(freetype_easy_print),
                        &SYM(MainMenu_Close),
                        &SYM(ShipGraph_Restart),
                        &SYM(PowerManager_RestartAll),
                        0 }
};
