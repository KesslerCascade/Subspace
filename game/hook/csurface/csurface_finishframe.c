#include "feature/screenshot.h"
#include "ftl/graphics/csurface.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CSurface_FinishFrame_pre(int* fb, float size_x, float size_y, bool aa)
{
    if (Screenshot_feature.enabled && gs.renderingScreenshot)
        return 0;       // don't swap buffers

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CSurface_FinishFrame,
                        subspace_CSurface_FinishFrame_pre,
                        NULL);
}

Patch patch_CSurface_FinishFrame = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CSurface_FinishFrame), 0 }
};
