#include "ftl/commandgui.h"

#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_CommandGui_GetCommand_post(int ret, CommandGui* self)
{
    return ret;
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CommandGui_GetCommand);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_GetCommand, NULL, subspace_CommandGui_GetCommand_post);
}

Patch patch_CommandGui_GetCommand = { .Relevant = AlwaysRequired,
                                      .Validate = validate,
                                      .Apply    = apply };
