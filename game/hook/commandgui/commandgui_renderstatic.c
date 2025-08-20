#include "ftl/commandgui.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_CommandGui_RenderStatic_post(CommandGui* self)
{
    // don't check feature state here because it might have been disabled after starting a practice
    // session
    practiceRenderNotice();
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_RenderStatic, NULL, subspace_CommandGui_RenderStatic_post);
}

Patch patch_CommandGui_RenderStatic = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CommandGui_RenderStatic), 0 }
};
