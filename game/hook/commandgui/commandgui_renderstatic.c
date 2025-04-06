#include "ftl/commandgui.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_CommandGui_RenderStatic_post(CommandGui* self) {}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, CommandGui_RenderStatic) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_RenderStatic, NULL, subspace_CommandGui_RenderStatic_post);
}

Patch patch_CommandGui_RenderStatic = { .Relevant = AlwaysRequired,
                                        .Validate = validate,
                                        .Apply    = apply };
