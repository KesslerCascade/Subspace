#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

#include "ftl/ftlbutton.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, FTLButton_OnRender);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, FTLButton_OnRender, subspace_FTLButton_OnRender_pre, NULL);
}

Patch patch_FTLButton_OnRender = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
