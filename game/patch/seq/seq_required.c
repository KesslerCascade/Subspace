#include "seq_required.h"
#include "patch/capp/capp.h"
#include "patch/commandgui/commandgui.h"

PatchSequence RequiredPatches = { &patch_CApp_OnExecute, 0 };
