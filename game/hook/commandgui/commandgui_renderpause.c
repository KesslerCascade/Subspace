#include "ftl/commandgui.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "feature/screenshot.h"

// ---- Hooks ----------------

int subspace_CommandGui_RenderPause_pre(CommandGui* self)
{
    if (Screenshot_feature.enabled && screenshotHidePause())
        return 0;

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CommandGui_RenderPause, subspace_CommandGui_RenderPause_pre, NULL);
}

Patch patch_CommandGui_RenderPause = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CommandGui_RenderPause), 0 }
};
