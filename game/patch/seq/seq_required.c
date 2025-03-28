#include "seq_required.h"
#include "patch/capp/capp.h"
#include "patch/commandgui/commandgui.h"

PatchSequence RequiredPatches = {
    &patch_CApp_OnExecute,
    &patch_CApp_OnLoop,
    &patch_CApp_OnKeyDown,
    &patch_CommandGui_KeyDown,
    0
};
