#include "ftl/capp.h"
#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CApp_OnExecute) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnExecute, subspace_CApp_OnExecute_pre, NULL);
}

Patch patch_CApp_OnExecute = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
