#include "control/controlclient.h"
#include "feature/timewarp.h"
#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

static int framecounter = 0;
int subspace_CApp_OnRender_pre(CApp* self)
{
    gs.frameCount++;

    if (TimeWarp_feature.enabled && gs.timeWarpActive && gs.warpFactor > 1) {
        framecounter++;
        if (framecounter >= (int)gs.warpFactor)
            framecounter = 0;
        else
            return 0;   // don't render this frame
    }

    double now = CFPS_GetTime(FPSControl);
    gs.renderCount++;

    if (now - gs.countStart >= 1000) {
        gs.countStart   = now;
        gs.lastFrame1s  = gs.frameCount;
        gs.lastRender1s = gs.renderCount;
        gs.frameCount = gs.renderCount = 0;
    }

    return 1;   // we do want to execute the original CApp::OnRender
}

// ---- Patch ----------------

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symResolve(base, CApp_OnRender) && symResolve(base, CFPS_GetTime) && symResolve(base, CFPS_FPSControl);
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base, CApp_OnRender, subspace_CApp_OnRender_pre, NULL);
}

Patch patch_CApp_OnRender = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };
