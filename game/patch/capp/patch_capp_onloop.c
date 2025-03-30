#include "ftl/capp.h"
#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnLoop) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnLoop, subspace_CApp_OnLoop_pre, subspace_CApp_OnLoop_post);
}

Patch patch_CApp_OnLoop = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
