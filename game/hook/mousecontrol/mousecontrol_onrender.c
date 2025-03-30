#include "ftl/stdlib.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/graphics/freetype.h"
#include "ftl/mousecontrol.h"
#include "subspacegame.h"

// This is just a convenient spot to render random stuff on the screen; since the mouse cursor is
// one of the last things that's drawn
extern double g_LastFrameTimeMS;

int subspace_MouseControl_OnRender_pre(MouseControl* self)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "Speed Factor: %f", CFPS_SpeedFactor(FPSControl));
    basic_string tmp;
    basic_string_set(&tmp, buf);
    easy_print(1, 100, 100, &tmp);
    basic_string_destroy(&tmp);

    snprintf(buf, sizeof(buf), "FPS: %d", CFPS_fps(FPSControl));
    basic_string_set(&tmp, buf);
    easy_print(1, 100, 115, &tmp);
    basic_string_destroy(&tmp);

    snprintf(buf, sizeof(buf), "Target frame time: %lfms", g_TargetFrameTimeMS);
    basic_string_set(&tmp, buf);
    easy_print(1, 100, 130, &tmp);
    basic_string_destroy(&tmp);

    if (gs.timeWarpActive) {
        snprintf(buf, sizeof(buf), "Last frame time: %lfms", g_LastFrameTimeMS);
        basic_string_set(&tmp, buf);
        easy_print(1, 100, 145, &tmp);
        basic_string_destroy(&tmp);
        snprintf(buf, sizeof(buf), "Warp Factor: %lf", gs.warpFactor);
        basic_string_set(&tmp, buf);
        easy_print(1, 100, 160, &tmp);
        basic_string_destroy(&tmp);
        snprintf(buf, sizeof(buf), "Warp Factor (Actual): %lf", gs.warpFactorActual);
        basic_string_set(&tmp, buf);
        easy_print(1, 100, 175, &tmp);
        basic_string_destroy(&tmp);
    }

    return 1;
}