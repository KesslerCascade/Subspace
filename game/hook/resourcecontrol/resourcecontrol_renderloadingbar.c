#include "control/controlclient.h"

#include "ftl/globals.h"
#include "ftl/resourcecontrol.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

static float lastPct = 0;
int subspace_ResourceControl_RenderLoadingBar_pre(ResourceControl* self, float initialProgress,
                                                  float finalProgress)
{
    if (finalProgress > 0) {
        int preload1          = ResourceControl_preLoadProgress1(self),
            preload2          = ResourceControl_preLoadProgress2(self);
        float preloadProgress = (float)preload1 / (float)preload2;
        float pct = initialProgress + preloadProgress * (finalProgress - initialProgress);

        if (pct != lastPct) {
            lastPct         = pct;
            ControlMsg* msg = controlNewMsg("Loading", 1);
            controlMsgFloat32(msg, 0, "pct", pct);
            controlClientQueue(msg);
        }
    }

    controlClientProcess();

    return 1;
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, ResourceControl_RenderLoadingBar) &&
        symResolve(base, ResourceControl_preLoadProgress1_offset) &&
        symResolve(base, ResourceControl_preLoadProgress2_offset) &&
        symResolve(base, ResourceControl_GlobalResources);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ResourceControl_RenderLoadingBar,
                        subspace_ResourceControl_RenderLoadingBar_pre,
                        NULL);
}

Patch patch_ResourceControl_RenderLoadingBar = { .Relevant = AlwaysRequired,
                                                 .Validate = validate,
                                                 .Apply    = apply };
