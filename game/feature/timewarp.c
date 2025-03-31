#include "timewarp.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "patch/seq/seq_timewarp.h"
#include "subspacegame.h"

static double baseFrameTime;
static double lastTime;
static int warpFrames;

static double warpTime;
static double realTime;

#define MAX_WARP_FACTOR 16
#define MIN_WARP_FACTOR 0.125

// ---- Control interface ----------------

// update all the necessary variables for the current warp rate
static void doTheTimeWarpAgain()
{
    // Adjust FPS target, but only for faster than normal time.
    // We cap the tick time at the original to ensure we don't go below 60 FPS when in slow motion
    // mode.
    g_TargetFrameTimeMS = MIN(baseFrameTime / gs.warpFactor, baseFrameTime);
    lastTime            = CFPS_GetTime(FPSControl);
    warpFrames          = 0;
    warpTime            = 0;
    realTime            = 0;
}

bool timeWarpBegin()
{
    if (!TimeWarp_feature.enabled)
        return false;
    if (gs.timeWarpActive)
        return true;

    gs.timeWarpActive = true;
    gs.warpFactor     = 1;
    doTheTimeWarpAgain();
    return true;
}

bool timeWarpIncrease()
{
    if (!timeWarpBegin())
        return false;

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MIN(gs.warpFactor * 2, MAX_WARP_FACTOR);
    doTheTimeWarpAgain();

    return (gs.warpFactor > oldfactor);
}

bool timeWarpDecrease()
{
    if (!timeWarpBegin())
        return false;

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MAX(gs.warpFactor / 2, MIN_WARP_FACTOR);
    doTheTimeWarpAgain();

    return (gs.warpFactor < oldfactor);
}

void timeWarpEnd()
{
    if (!gs.timeWarpActive)
        return;

    gs.timeWarpActive   = false;
    gs.warpFactor       = 1;
    gs.warpFactorActual = 1;
    g_TargetFrameTimeMS = baseFrameTime;   // restore frame time for FPS limiter
}

// ---- Warp implementation ----------------

float timeWarpAdjustSpeedFactor(float orig)
{
    // SpeedFactor is capped to 0.5 to prevent things like projectiles warping past shields
    float speedFactor = MIN(orig * gs.warpFactor, 0.5);

    warpFrames++;
    int warpFrameCheck = MAX((int)(5 * gs.warpFactor), 5);

    // calculate the actual warp factor based on real time elapsed vs advanced game time,
    // but don't do it every frame because GetTime calls QueryPerformanceCounter a couple times
    if (warpFrames % warpFrameCheck == warpFrameCheck - 1) {
        double now       = CFPS_GetTime(FPSControl);
        double frameTime = (now - lastTime);
        lastTime         = now;

        warpTime += frameTime * speedFactor;
        realTime += frameTime * orig;
        gs.warpFactorActual = (realTime > 0) ? warpTime / realTime : 0;

        // fine-tune the frame time to try to reach our target FPS, but only for faster than normal
        // warps
        if (gs.warpFactor > 1) {
            double frameTimeAdj   = 0;
            double idealFrameTime = MIN(baseFrameTime / gs.warpFactor, baseFrameTime);

            gs.avgFrameMS = frameTime /
                (double)warpFrameCheck;   // actually the average over the last few frames
            if (gs.avgFrameMS > idealFrameTime * 1.02)
                frameTimeAdj = -gs.avgFrameMS / 1000;
            else if (gs.avgFrameMS < idealFrameTime * 0.98)
                frameTimeAdj = gs.avgFrameMS / 1000;

            g_TargetFrameTimeMS = MAX(MIN(g_TargetFrameTimeMS + frameTimeAdj, baseFrameTime), 0);
        }
    }

    return speedFactor;
}

void timeWarpBeginFrame(CommandGui* gui)
{
    if (gs.timeWarpActive) {
        // time warp always ends when the game is paused
        if (CommandGui_IsPaused(gui))
            timeWarpEnd();
    }
}

// ---- Patching ----------------

static bool timeWarp_Patch(SubspaceFeature* feat, void* settings, PatchState* ps)
{
    bool ret = patchApplySeq(ps, TimeWarpPatches);
    if (ret)
        baseFrameTime = g_TargetFrameTimeMS;
    return ret;
}

static bool timeWarp_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    if (!enabled)
        gs.frameAdvStep = false;

    return enabled;
}

SubspaceFeature TimeWarp_feature = {
    .patch           = timeWarp_Patch,
    .enable          = timeWarp_Enable,
    .requiredSymbols = { &SYM(CFPS_FPSControl),
                        &SYM(CFPS_GetTime),
                        &SYM(CFPS_fps_offset),
                        &SYM(TargetFrameTimeMS),
                        0 }
};
