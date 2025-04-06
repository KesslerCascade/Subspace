#include "feature/numerichull.h"
#include "ftl/combatcontrol.h"
#include "ftl/shipmanager.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CombatControl_RenderTarget_pre(CombatControl* self)
{
    if (NumericHull_feature.enabled) {
        ShipManager* sm = CombatControl_GetCurrentTarget(self);
        if (sm)
            numericHullBeforeRenderTarget(sm);
    }

    return 1;
}

void subspace_CombatControl_RenderTarget_post(CombatControl* self)
{
    if (NumericHull_feature.enabled)
        numericHullAfterRenderTarget();
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CombatControl_RenderTarget) &&
        symResolve(base, CombatControl_GetCurrentTarget) &&
        symResolve(base, ShipManager_ship_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CombatControl_RenderTarget,
                        subspace_CombatControl_RenderTarget_pre,
                        subspace_CombatControl_RenderTarget_post);
}

Patch patch_CombatControl_RenderTarget = { .Relevant = AlwaysRequired,
                                           .Validate = validate,
                                           .Apply    = apply };
