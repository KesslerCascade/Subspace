#include "seq_capp.h"

extern Patch patch_CApp_OnExecute;

PatchSequence CAppPatches = { &patch_CApp_OnExecute, 0 };
