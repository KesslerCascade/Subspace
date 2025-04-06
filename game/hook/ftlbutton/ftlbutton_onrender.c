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

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, FTLButton_OnRender);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, FTLButton_OnRender, subspace_FTLButton_OnRender_pre, NULL);
}

Patch patch_FTLButton_OnRender = { .Relevant = AlwaysRequired,
                                   .Validate = validate,
                                   .Apply    = apply };
