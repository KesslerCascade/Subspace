#include "seq_timewarp.h"
#include "patch/cfps/cfps.h"
#include "patch/commandgui/commandgui.h"
#include "patch/mousecontrol/mousecontrol.h"

PatchSequence TimeWarpPatches = { &patch_CFPS_OnLoop,           &patch_CFPS_TargetTickTime,
                                  &patch_MouseControl_OnRender, &patch_CommandGui_KeyDown,
                                  &patch_CommandGui_OnLoop,     0 };
