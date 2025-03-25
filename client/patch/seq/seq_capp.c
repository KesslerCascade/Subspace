#include "seq_capp.h"

extern Patch patch_CApp_OnExecute;
extern Patch patch_CApp_OnLoop;

PatchSequence CAppPatches = { &patch_CApp_OnExecute, &patch_CApp_OnLoop, 0 };
