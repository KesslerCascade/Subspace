#include "ftl/capp.h"
#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnKeyDown) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnKeyDown, subspace_CApp_OnKeyDown_pre, NULL);
}

Patch patch_CApp_OnKeyDown = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
