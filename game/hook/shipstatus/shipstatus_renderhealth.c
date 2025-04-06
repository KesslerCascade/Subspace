#include "feature/numerichull.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_ShipStatus_RenderHealth_pre(ShipStatus* self, bool renderText)
{
    if (NumericHull_feature.enabled) {
        ShipManager* sm = ShipStatus_ship(self);
        if (sm)
            numericHullBeforeRenderHealth(sm, renderText);
    }
    return 1;
}

void subspace_ShipStatus_RenderHealth_post(ShipStatus* self, bool renderText)
{
    if (NumericHull_feature.enabled)
        numericHullAfterRenderHealth(renderText);
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ShipStatus_RenderHealth) && symResolve(base, ShipStatus_ship_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ShipStatus_RenderHealth,
                        subspace_ShipStatus_RenderHealth_pre,
                        subspace_ShipStatus_RenderHealth_post);
}

Patch patch_ShipStatus_RenderHealth = { .Relevant = AlwaysRequired,
                                        .Validate = validate,
                                        .Apply    = apply };
