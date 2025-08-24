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

void subspace_CApp_OnRender_post(CApp* self)
{
    if (Screenshot_feature.enabled && gs.screenshotNow)
        renderScreenshot(self);
    gs.screenshotNow = false;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CApp_OnRender,
                        subspace_CApp_OnRender_pre,
                        subspace_CApp_OnRender_post);
}

Patch patch_CApp_OnRender = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CApp_OnRender), &SYM(CFPS_GetTime), &SYM(CFPS_FPSControl), 0 }
};
