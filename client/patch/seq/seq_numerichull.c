#include "seq_numerichull.h"
#include "patch/ship/ship.h"
#include "patch/shipstatus/shipstatus.h"
#include "patch/textlibrary/textlibrary.h"

PatchSequence NumericHullPatches = {
    &validate_Ship,
    &validate_ShipStatus,
    &patch_TextLibrary_GetText,
    &patch_ShipStatus_RenderHealth,
    0
};
