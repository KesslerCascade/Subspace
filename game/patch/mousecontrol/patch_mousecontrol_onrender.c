#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"
#include "mousecontrol/mousecontrol.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, MouseControl_OnRender) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, MouseControl_OnRender, subspace_MouseControl_OnRender_pre, NULL);
}

Patch patch_MouseControl_OnRender = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
