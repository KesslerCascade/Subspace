#include "feature/frameadv.h"
#include "feature/timewarp.h"
#include "ftl/commandgui.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CommandGui_OnLoop_pre(CommandGui* self)
{
    if (TimeWarp_feature.enabled) {
        timeWarpBeginFrame(self);
    }
    return 1;   // we should probably run CommandGui ;)
}

void subspace_CommandGui_OnLoop_post(CommandGui* self)
{
    if (FrameAdv_feature.enabled)
        frameAdvEndFrame(self);
}

// ---- Patch ----------------

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

Patch patch_CommandGui_OnLoop = { .Relevant = AlwaysRequired,
                                  .Validate = validate,
                                  .Apply    = apply };
