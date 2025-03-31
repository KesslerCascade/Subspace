#include "ftl/stdlib.h"

#include "patch/seq/seq_numerichull.h"
#include "numerichull.h"

void numericHullBeforeRenderHealth(ShipManager* sm, bool renderText)
{
    Ship* ship = ShipManager_ship(sm);

    if (ship) {
        gs.overrideHullText = true;
        gs.hull             = MEMBER(ftlbase, Ship, ship, int, hullIntegrity);
    }
}

void numericHullAfterRenderHealth(bool renderText)
{
    gs.overrideHullText = false;
}

void numericHullBeforeRenderTarget(ShipManager* smtgt)
{
    Ship* ship = ShipManager_ship(smtgt);

    if (ship) {
        gs.overrideHullText = true;
        gs.hull             = MEMBER(ftlbase, Ship, ship, int, hullIntegrity);
    }
}

void numericHullAfterRenderTarget(void)
{
    gs.overrideHullText = false;
}

bool numericHullCheckText(TextLibrary* tlib, basic_string* text, basic_string* out)
{
    if (gs.overrideHullText && text && !strcmp(text->buf, "status_hull")) {
        char buf[32];
        snprintf(buf, 32, "HULL: %d", gs.hull);
        basic_string_set(out, buf);
        return true;
    }
    return false;
}

// ---- Patching ----------------

static bool numericHull_Patch(SubspaceFeature* feat, void* settings, PatchState* ps)
{
    return patchApplySeq(ps, NumericHullPatches);
}

SubspaceFeature NumericHull_feature = {
    .patch           = numericHull_Patch,
    .requiredSymbols = { &SYM(ShipManager_ship_offset), &SYM(Ship_hullIntegrity_offset), 0 }
};
