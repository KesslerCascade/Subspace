#include "feature/infoblock.h"
#include "ftl/ftlbutton.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

int subspace_FTLButton_OnRender_pre(FTLButton* self)
{
    if (InfoBlock_feature.enabled)
        infoBlockRender();

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, FTLButton_OnRender, subspace_FTLButton_OnRender_pre, NULL);
}

Patch patch_FTLButton_OnRender = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(FTLButton_OnRender), 0 }
};
