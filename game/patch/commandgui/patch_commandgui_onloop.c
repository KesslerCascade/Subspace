#include "ftl/commandgui.h"
#include "ftl/ftl.h"
#include "hook/hook.h"
#include "patch/patch.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CommandGui_OnLoop) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CommandGui_OnLoop,
                        subspace_CommandGui_OnLoop_pre,
                        subspace_CommandGui_OnLoop_post);
}

Patch patch_CommandGui_OnLoop = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
