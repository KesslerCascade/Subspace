#include "feature/timewarp.h"
#include "feature/tweaks.h"
#include "ftl/cfps.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

void subspace_CFPS_OnLoop_post(CFPS* self)
{
    if (gs.tweaksOverrideFPS)
        tweaksSetFrameTime();

    if (TimeWarp_feature.enabled && gs.timeWarpActive) {
        CFPS_SpeedFactor(self) = timeWarpAdjustSpeedFactor(CFPS_SpeedFactor(self));
    }

    if (gs.frameAdvStep) {
        // If we're frame stepping, override speed factor to get a consistent step regardless of
        // actual FPS. This speed factor is equivalent to 1 frame of the game running at a perfect
        // 60fps.
        CFPS_SpeedFactor(FPSControl) = 16. / 60.;
    }
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CFPS_OnLoop) && symResolve(base, CFPS_SpeedFactor_offset);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CFPS_OnLoop, NULL, subspace_CFPS_OnLoop_post);
}

Patch patch_CFPS_OnLoop = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
