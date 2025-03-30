#include "timewarp.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "subspacegame.h"

static bool init;
static double baseFrameTime;

void timeWarpBegin()
{
    if (gs.timeWarpActive)
        return;

    if (!init) {
        baseFrameTime = g_TargetFrameTimeMS;
        init          = true;
    }

    gs.timeWarpActive = true;
    gs.warpFactor     = 1;
}

// update all the necessary variables for the current warp rate
static void doTheTimeWarpAgain()
{
    // Adjust FPS target, but only for faster than normal time.
    // We cap the tick time at the original to ensure we don't go below FPS when in slow motion
    // mode.
    g_TargetFrameTimeMS = MIN(baseFrameTime / gs.warpFactor, baseFrameTime);
}

bool timeWarpIncrease()
{
    timeWarpBegin();

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MIN(gs.warpFactor * 2, 32);
    doTheTimeWarpAgain();

    return (gs.warpFactor > oldfactor);
}

bool timeWarpDecrease()
{
    timeWarpBegin();

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MAX(gs.warpFactor / 2, 0.125);
    doTheTimeWarpAgain();

    return (gs.warpFactor < oldfactor);
}

void timeWarpEnd()
{
    if (!gs.timeWarpActive)
        return;

    gs.timeWarpActive   = false;
    g_TargetFrameTimeMS = baseFrameTime;
}

void timeWarpFrameCalcActual(float speedFactor)
{
    // reverse engineer the actual warp factor from the current speed factor and FPS
    double fps          = CFPS_fps(FPSControl);
    double frametime    = fps * speedFactor;
    gs.warpFactorActual = frametime / baseFrameTime;

    // adjust target frame time as necessary to try to get our desired framerate
    double targetfps    = MAX(60 * gs.warpFactor, 60);
    double frameTimeAdj = 0;
    if (fps < targetfps * 0.98)
        frameTimeAdj = -0.001 * (targetfps - fps) / targetfps;
    else if (fps > targetfps * 1.02)
        frameTimeAdj = 0.001 * (fps - targetfps) / targetfps;

    g_TargetFrameTimeMS = MAX(MIN(g_TargetFrameTimeMS + frameTimeAdj, baseFrameTime), 0);
}
