#include "seq_numerichull.h"
#include "patch/combatcontrol/combatcontrol.h"
#include "patch/shipstatus/shipstatus.h"
#include "patch/textlibrary/textlibrary.h"

PatchSequence NumericHullPatches = { &patch_TextLibrary_GetText,
                                     &patch_ShipStatus_RenderHealth,
                                     &patch_CombatControl_RenderTarget,
                                     0 };
