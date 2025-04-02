#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/shipstatus.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ShipStatus_OnRender);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, ShipStatus_OnRender, NULL, subspace_ShipStatus_OnRender_post);
}

Patch patch_ShipStatus_OnRender = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
