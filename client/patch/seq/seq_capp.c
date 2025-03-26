#include "seq_capp.h"

extern Patch patch_CApp_OnExecute;
extern Patch patch_CApp_OnLoop;
extern Patch patch_CApp_OnKeyDown;

PatchSequence CAppPatches = { &patch_CApp_OnExecute, &patch_CApp_OnLoop, &patch_CApp_OnKeyDown, 0 };
