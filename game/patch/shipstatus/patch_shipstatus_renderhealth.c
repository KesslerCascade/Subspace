#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/shipstatus.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ShipStatus_RenderHealth) && symResolve(base, ShipStatus_ship_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, ShipStatus_RenderHealth, subspace_ShipStatus_RenderHealth_pre, subspace_ShipStatus_RenderHealth_post);
}

Patch patch_ShipStatus_RenderHealth = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
