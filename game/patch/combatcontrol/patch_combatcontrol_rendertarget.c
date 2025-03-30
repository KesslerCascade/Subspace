#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/combatcontrol.h"
#include "ftl/shipmanager.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CombatControl_RenderTarget) &&
    symResolve(base, CombatControl_GetCurrentTarget) &&
    symResolve(base, ShipManager_ship_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CombatControl_RenderTarget, subspace_CombatControl_RenderTarget_pre, subspace_CombatControl_RenderTarget_post);
}

Patch patch_CombatControl_RenderTarget = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
