#include "feature/timewarp.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

void subspace_ShipStatus_OnRender_post(ShipStatus* self)
{
    if (TimeWarp_feature.enabled)
        timeWarpRender();
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, ShipStatus_OnRender, NULL, subspace_ShipStatus_OnRender_post);
}

Patch patch_ShipStatus_OnRender = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ShipStatus_OnRender), 0 }
};
