#include "seq_frameadv.h"
#include "patch/commandgui/commandgui.h"

PatchSequence FrameAdvPatches = {
    &patch_CommandGui_KeyDown,
    &patch_CommandGui_OnLoop,
    0
};
