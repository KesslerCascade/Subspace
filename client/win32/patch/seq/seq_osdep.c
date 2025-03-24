#include "seq_osdep.h"

extern Patch patch_OSDepWinMain;

PatchSequence OSDepPatches = { &patch_OSDepWinMain, 0 };
