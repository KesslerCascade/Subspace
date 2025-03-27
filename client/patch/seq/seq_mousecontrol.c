#include "seq_mousecontrol.h"

extern Patch patch_MouseControl_OnRender;

PatchSequence MouseControlPatches = { &patch_MouseControl_OnRender, 0 };
