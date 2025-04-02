#include "ftl/stdlib.h"

#include "timewarp.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "patch/seq/seq_timewarp.h"
#include "subspacegame.h"

static double baseFrameTime;
static double lastTime;
static int warpFrames;

static double warpTime;
static double realTime;

static float colorTimer     = 0;
static float animationTimer = 0;
static float fadeTimer      = 16;

static const GL_Color warpTextColors[2] = {
    { 1, 0.715, 0, 1 },
    { 1, 0.898, 0, 1 }
};

#define MAX_WARP_FACTOR 16
#define MIN_WARP_FACTOR 0.125

#define TIMEWARP_FONT         24
#define TIMEWARP_CHEVRON_FONT 18

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
    fadeTimer         = 0;
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

    if (gs.warpFactor > 0.99 && gs.warpFactor < 1.01)
        timeWarpEnd();

    if (fadeTimer > 16)
        fadeTimer = 2;

    return (gs.warpFactor > oldfactor);
}

bool timeWarpDecrease()
{
    if (!timeWarpBegin())
        return false;

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MAX(gs.warpFactor / 2, MIN_WARP_FACTOR);
    doTheTimeWarpAgain();

    if (gs.warpFactor > 0.99 && gs.warpFactor < 1.01)
        timeWarpEnd();

    if (fadeTimer > 16)
        fadeTimer = 2;

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
    fadeTimer           = 0;
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

#define lerp(a, b, t) ((a) * (1 - (t)) + (b) * (t))
void timeWarpRender()
{
    if (gs.timeWarpActive || fadeTimer < 16) {
        float speedFactorReal = CFPS_SpeedFactor(FPSControl);

        if (gs.timeWarpActive)
            animationTimer += speedFactorReal;   // animation takes warp factor into account!

        if (gs.warpFactor > 0.1) {
            speedFactorReal /= gs.warpFactor;
        }

        colorTimer += speedFactorReal;
        fadeTimer += speedFactorReal;

        GL_Color textcolor;
        float abounce = sin(colorTimer / 32 * 3.1415) / 2 + 1;
        textcolor.r   = lerp(warpTextColors[0].r, warpTextColors[1].r, abounce);
        textcolor.g   = lerp(warpTextColors[0].g, warpTextColors[1].g, abounce);
        textcolor.b   = lerp(warpTextColors[0].b, warpTextColors[1].b, abounce);

        if (gs.timeWarpActive) {
            if (fadeTimer < 2)
                textcolor.a = fadeTimer / 2;
            else if (fadeTimer < 18)
                textcolor.a = 1;
            else if (fadeTimer < 82)
                textcolor.a = ((82 - fadeTimer) / 256) + 0.75;
            else
                textcolor.a = 0.75;
        } else {
            textcolor.a = MAX(1 - (fadeTimer / 16), 0);
        }

        char buf[64];
        if (gs.timeWarpActive)
            snprintf(buf, sizeof(buf), "Time Warp: %gx", gs.warpFactor);
        else
            strcpy(buf, "Time Warp: Canceled!");

        basic_string tmp;
        basic_string_set(&tmp, buf);
        CSurface_GL_SetColor(textcolor);
        easy_printCenter(TIMEWARP_FONT, 640, 95, &tmp);
        basic_string_destroy(&tmp);

        const char* animtext = "> ";

        basic_string_set(&tmp, animtext);
        Pointf atsz = easy_measurePrintLines(TIMEWARP_CHEVRON_FONT, 0, 0, 1000, &tmp);
        basic_string_destroy(&tmp);

        while (animationTimer > 32)   // 2 second cycle
            animationTimer -= 32;

        basic_string_set(&tmp, animtext);
        CSurface_GL_SetColor(textcolor);
        float axs = 640 - atsz.x * 9.5 / 2;
        float ay  = 130;
        for (int i = 0; i < 7; i++) {
            easy_print(TIMEWARP_CHEVRON_FONT,
                       axs + atsz.x * (i + 1) + (animationTimer / 32 * atsz.x),
                       ay,
                       &tmp);
        }

        GL_Color sidecolor = textcolor;
        sidecolor.a        = textcolor.a * animationTimer / 32;
        CSurface_GL_SetColor(sidecolor);
        easy_print(TIMEWARP_CHEVRON_FONT, axs + (animationTimer / 32 * atsz.x), ay, &tmp);

        sidecolor.a = textcolor.a * (32 - animationTimer) / 32;
        CSurface_GL_SetColor(sidecolor);
        easy_print(TIMEWARP_CHEVRON_FONT,
                   axs + atsz.x * 8 + (animationTimer / 32 * atsz.x),
                   ay,
                   &tmp);
        basic_string_destroy(&tmp);
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
                        &SYM(freetype_easy_printCenter),
                        &SYM(freetype_easy_measurePrintLines),
                        &SYM(msvcrt_sin),
                        &SYM(msvcrt_snprintf),
                        0 }
};
