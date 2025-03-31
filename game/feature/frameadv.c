#include "frameadv.h"
#include "ftl/commandgui.h"
#include "patch/seq/seq_frameadv.h"

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

static bool frameAdv_Patch(SubspaceFeature* feat, void* settings, PatchState* ps)
{
    return patchApplySeq(ps, FrameAdvPatches);
}

static bool frameAdv_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    if (!enabled)
        gs.frameAdvStep = false;

    return enabled;
}

SubspaceFeature FrameAdv_feature = {
    .patch           = frameAdv_Patch,
    .enable          = frameAdv_Enable,
    .requiredSymbols = { &SYM(CommandGui_SetPaused), 0 }
};
