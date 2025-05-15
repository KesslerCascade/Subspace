#include "frameadv.h"
#include "feature/feature.h"
#include "ftl/cfps.h"
#include "ftl/commandgui.h"
#include "ftl/globals.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"

void frameAdvStep(CommandGui* gui)
{
    // unpause the game, but set a flag to re-pause at the end of the frame
    CommandGui_SetPaused(gui, false, false);
    gs.frameAdvStep = true;
}

void frameAdvEndFrame(CommandGui* gui)
{
    if (gs.frameAdvStep) {
        CommandGui_SetPaused(gui, true, false);
        gs.frameAdvStep = false;
    }
}

// ---- Keybinds ----------------

static void frameadv_key_advance_cb(int key, int flags)
{
    frameAdvStep(CApp_gui(theApp));
}

static KeyBind FrameAdv_keybinds[] = {
    { .name      = "frameadv_advance",
     .context   = KB_CTX_GAME,
     .flags_exc = KB_JUMPING,
     .func      = frameadv_key_advance_cb },
    { 0 }
};

// ---- Patching ----------------

static bool frameAdv_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    if (!enabled)
        gs.frameAdvStep = false;

    return enabled;
}

Patch* FrameAdv_patches[] = { &patch_CommandGui_KeyDown,
                              &patch_CommandGui_OnLoop,
                              &patch_CFPS_OnLoop,
                              0 };

SubspaceFeature FrameAdv_feature = {
    .name            = "FrameAdv",
    .enable          = frameAdv_Enable,
    .keybinds        = FrameAdv_keybinds,
    .requiredPatches = FrameAdv_patches,
    .requiredSymbols = { &SYM(CommandGui_SetPaused),
                        &SYM(CApp_gui_offset),
                        &SYM(CFPS_FPSControl),
                        &SYM(CFPS_SpeedFactor_offset),
                        0 }
};
