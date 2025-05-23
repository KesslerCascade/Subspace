#include "ftl/stdlib.h"

#include "timewarp.h"

#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"
#include "control.h"
#include "subspacegame.h"

static double baseFrameTime = 16.66666666666667;
static double lastTime;
static int warpFrames;

static double warpTime;
static double realTime;
static double avgFrameMS;   // milliseconds per frame, averaged over the last several frames

static float colorTimer     = 0;
static float animationTimer = 0;
static float fadeTimer      = 16;

static char savedFrameLimit;   // whether frame limiter was on or off when time warp began

static bool firstTap = false;

static const GL_Color warpTextColors[2] = {
    { 1, 0.715, 0, 1   },
    { 1, 0.898, 0, 0.9 }
};

#define MAX_WARP_FACTOR 32
#define MIN_WARP_FACTOR 0.125

#define TIMEWARP_FONT         24
#define TIMEWARP_CHEVRON_FONT 18

// ---- Control interface ----------------

// update all the necessary variables for the current warp rate
static void doTheTimeWarpAgain()
{
    CommandGui* gui = CApp_gui(theApp);

    // unpause if a time warp key was pressed while paused
    if (CommandGui_IsPaused(gui)) {
        CommandGui_SetPaused(gui, 0, 0);
    }

    // Adjust FPS target, but only for faster than normal time.
    // We cap the tick time at the original to ensure we don't go below 60 FPS when in slow
    // motion mode.
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

    savedFrameLimit = g_opt_framelimit;   // save frame limiter state in case we need to change it

    baseFrameTime     = g_TargetFrameTimeMS;   // capture current frame time (i.e. from tweaks)
    gs.timeWarpActive = true;
    gs.warpFactor     = 1;
    fadeTimer         = 0;
    doTheTimeWarpAgain();
    return true;
}

bool timeWarpIncrease()
{
    TimeWarpSettings* settings = TimeWarp_feature.settings;

    // special case: double tap set to 2x means we have to check before initiating the warp
    if (settings->doubletap == 2 && !gs.timeWarpActive && !firstTap) {
        firstTap = true;
        return false;
    }

    if (!timeWarpBegin())
        return false;

    if (settings->doubletap != 0 && gs.warpFactor * 2 + 0.01 >= settings->doubletap && !firstTap) {
        firstTap = true;
        return false;
    }

    firstTap = false;

    float oldfactor = gs.warpFactor;
    gs.warpFactor   = MIN(MIN(gs.warpFactor * 2, MAX_WARP_FACTOR), settings->maxwarp);
    doTheTimeWarpAgain();

    if (gs.warpFactor > 0.99 && gs.warpFactor < 1.01)
        timeWarpEnd();

    // Warp factors > 1 use frameskip warp which REQUIRES that the frame limiter be enabled.
    // Otherwise if vsync is on, there will be a burst of frames at the maximum rate, then a long
    // pause waiting for vsync. This causes CFPS calculation to get all screwed up and advance
    // the time step far slower than it should.
    if (gs.warpFactor > 1)
        g_opt_framelimit = 1;

    if (fadeTimer > 16)
        fadeTimer = 2;

    return (gs.warpFactor > oldfactor);
}

bool timeWarpDecrease()
{
    TimeWarpSettings* settings = TimeWarp_feature.settings;
    if (!settings->allowslowmo && (!gs.timeWarpActive || gs.warpFactor / 2 < 0.99)) {
        return false;
    }

    if (!timeWarpBegin())
        return false;

    firstTap = false;

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

    g_opt_framelimit = savedFrameLimit;

    gs.timeWarpActive   = false;
    gs.warpFactor       = 1;
    gs.warpFactorActual = 1;
    g_TargetFrameTimeMS = baseFrameTime;   // restore frame time for FPS limiter
    fadeTimer           = 0;
    firstTap            = false;
}

// ---- Warp implementation ----------------

float timeWarpAdjustSpeedFactor(float orig)
{
    // SpeedFactor is capped to 0.5 to prevent things like projectiles warping past shields
    float speedFactor = MIN(orig * gs.warpFactor, 0.5);

    warpFrames++;
    int warpFrameCheck = MAX((int)(2 * gs.warpFactor), 5);

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

            avgFrameMS = frameTime /
                (double)warpFrameCheck;   // actually the average over the last few frames
            if (avgFrameMS > idealFrameTime * 1.02)
                frameTimeAdj = -avgFrameMS / 1000;
            else if (avgFrameMS < idealFrameTime * 0.98)
                frameTimeAdj = avgFrameMS / 1000;

            g_TargetFrameTimeMS = MAX(MIN(g_TargetFrameTimeMS + frameTimeAdj, baseFrameTime), 0);
        }
    }

    return speedFactor;
}

void timeWarpBeginFrame(CommandGui* gui)
{
    if (gs.timeWarpActive || fadeTimer < 16) {
        // time warp always ends when the game is paused
        if (CommandGui_IsPaused(gui)) {
            timeWarpEnd();
            fadeTimer = 16;   // and instantly hide the text so it doesn't overlap with "Paused"
        }
    }
}

#define lerp(a, b, t) ((a) * (1 - (t)) + (b) * (t))
void timeWarpRender()
{
    if (gs.timeWarpActive || fadeTimer < 16) {
        float speedFactorReal = CFPS_SpeedFactor(FPSControl);

        // In time warps > 1 we're also frame skipping, so timeWarpRender wlil be called less often,
        // and we need to adjust for that
        if (gs.warpFactor > 1)
            speedFactorReal *= (int)gs.warpFactor;

        if (gs.timeWarpActive)
            animationTimer += speedFactorReal;   // animation takes warp factor into account!

        if (gs.warpFactor > 0.1) {
            speedFactorReal /= gs.warpFactor;   // cancel out the warp factor for color & fade
                                                // timers
        }

        colorTimer += speedFactorReal;
        fadeTimer += speedFactorReal;

        GL_Color textcolor;
        float abounce = sin(colorTimer / 32 * 3.1415) / 2 + 1;
        textcolor.r   = lerp(warpTextColors[0].r, warpTextColors[1].r, abounce);
        textcolor.g   = lerp(warpTextColors[0].g, warpTextColors[1].g, abounce);
        textcolor.b   = lerp(warpTextColors[0].b, warpTextColors[1].b, abounce);
        textcolor.a   = lerp(warpTextColors[0].a, warpTextColors[1].a, abounce);

        float fade = 1;
        if (gs.timeWarpActive) {
            if (fadeTimer < 2)
                fade = fadeTimer / 2;
            else if (fadeTimer < 18)
                fade = 1;
            else if (fadeTimer < 82)
                fade = ((82 - fadeTimer) / 427) + 0.85;
            else
                textcolor.a = 0.85;
        } else {
            textcolor.a = MAX(1 - (fadeTimer / 16), 0);
        }
        textcolor.a *= fade;

        char buf[64];
        if (gs.timeWarpActive)
            snprintf(buf, sizeof(buf), "Time Warp: %gx", gs.warpFactor);
        else
            strcpy(buf, "Time Warp: Canceled!");

        basic_string tmp;
        basic_string_set(&tmp, buf);
        CSurface_GL_SetColor(textcolor);
        easy_printCenter(TIMEWARP_FONT, 640, 535, &tmp);
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
        float ay  = 570;
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

// ---- Keybinds ----------------

static void timewarp_key_increase_cb(int key, int flags)
{
    timeWarpIncrease();
}

static void timewarp_key_decrease_cb(int key, int flags)
{
    timeWarpDecrease();
}

static void timewarp_key_cancel_cb(int key, int flags)
{
    timeWarpEnd();
}

static KeyBind TimeWarp_keybinds[] = {
    { .name      = "timewarp_increase",
     .context   = KB_CTX_GAME,
     .flags_exc = KB_JUMPING,
     .func      = timewarp_key_increase_cb },
    { .name      = "timewarp_decrease",
     .context   = KB_CTX_GAME,
     .flags_exc = KB_JUMPING,
     .func      = timewarp_key_decrease_cb },
    { .name      = "timewarp_cancel",
     .context   = KB_CTX_GAME,
     .flags_exc = KB_JUMPING | KB_PAUSED,
     .func      = timewarp_key_cancel_cb },
    { 0 }
};

// ---- Patching ----------------

static bool timeWarp_Patch(SubspaceFeature* feat, void* settings, PatchState* ps)
{
    return true;
}

static bool timeWarp_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    if (!enabled)
        timeWarpEnd();

    return enabled;
}

Patch* TimeWarp_patches[] = { &patch_CFPS_OnLoop,           &patch_CFPS_TargetFrameTime,
                              &patch_MouseControl_OnRender, &patch_CommandGui_KeyDown,
                              &patch_CommandGui_OnLoop,     &patch_ShipStatus_OnRender,
                              &patch_CApp_OnRender,         0 };

FeatureSettingsSpec TimeWarp_spec = {
    .size = sizeof(TimeWarpSettings),
    .ent  = { { .name = "maxwarp", .type = CF_INT, .off = offsetof(TimeWarpSettings, maxwarp) },
             { .name = "allowslowmo",
                .type = CF_BOOL,
                .off  = offsetof(TimeWarpSettings, allowslowmo) },
             { .name = "doubletap", .type = CF_INT, .off = offsetof(TimeWarpSettings, doubletap) },
             { 0 } }
};

SubspaceFeature TimeWarp_feature = {
    .name            = "TimeWarp",
    .patch           = timeWarp_Patch,
    .enable          = timeWarp_Enable,
    .keybinds        = TimeWarp_keybinds,
    .settingsspec    = &TimeWarp_spec,
    .requiredPatches = TimeWarp_patches,
    .requiredSymbols = { &SYM(CFPS_FPSControl),
                        &SYM(CFPS_GetTime),
                        &SYM(CFPS_fps_offset),
                        &SYM(CFPS_SpeedFactor_offset),
                        &SYM(TargetFrameTimeMS),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(freetype_easy_printCenter),
                        &SYM(freetype_easy_measurePrintLines),
                        &SYM(CommandGui_IsPaused),
                        &SYM(CommandGui_SetPaused),
                        &SYM(CApp_gui_offset),
                        &SYM(opt_framelimit),
                        &SYM(crt_sin),
                        &SYM(crt_snprintf),
                        0 }
};
