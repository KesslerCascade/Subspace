#include "frameadv.h"
#include "ftl/commandgui.h"
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

// ---- Patching ----------------

static bool frameAdv_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    if (!enabled)
        gs.frameAdvStep = false;

    return enabled;
}

Patch* FrameAdv_patches[] = { &patch_CommandGui_KeyDown, &patch_CommandGui_OnLoop, 0 };

SubspaceFeature FrameAdv_feature = {
    .enable          = frameAdv_Enable,
    .requiredPatches = FrameAdv_patches,
    .requiredSymbols = { &SYM(CommandGui_SetPaused), 0 }
};
