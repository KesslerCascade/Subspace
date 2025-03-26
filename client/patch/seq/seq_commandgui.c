#include "seq_commandgui.h"

extern Patch patch_CommandGui_KeyDown;

PatchSequence CommandGuiPatches = { &patch_CommandGui_KeyDown, 0 };
