#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CFPS_OnLoop) &&
    symResolve(base, CFPS_SpeedFactor_offset) &&
    symResolve(base, TargetFrameTimeMS);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CFPS_OnLoop, NULL, subspace_CFPS_OnLoop_post);
}

Patch patch_CFPS_OnLoop = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
