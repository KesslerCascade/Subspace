#include "seq_startup.h"

extern Patch patch_StartupMain;

PatchSequence StartupPatches = { &patch_StartupMain, 0 };
