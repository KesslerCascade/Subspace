#include "control.h"

#include "ftl/stdlib.h"

#include "feature/feature.h"
#include "feature/timewarp.h"
#include "ftl/globals.h"
#include "ftl/graphics/colors.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/struct.h"
#include "patch/patchlist.h"
#include "infoblock.h"

#define INFOBLOCK_FONT 6

void infoBlockRender(void)
{
    InfoBlockSettings* settings = (InfoBlockSettings*)InfoBlock_feature.settings;

    char buf[128];
    basic_string tmp;
    Pointf sz;
    float x = 1280 - 5;
    float y = 5;

    CSurface_GL_SetColor(COLOR_WHITE);

    for (int i = 0; i < 3; i++) {
        if (settings->ssver == i) {
#ifdef _DEBUG
            snprintf(buf, sizeof(buf), "Subspace %s (DEBUG BUILD)", subspace_version_str);
#else
            snprintf(buf, sizeof(buf), "Subspace %s", subspace_version_str);
#endif
            basic_string_set(&tmp, buf);
            sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y  = sz.y;
            basic_string_destroy(&tmp);
        }

        if (settings->ftlver == i) {
            snprintf(buf,
                     sizeof(buf),
                     "FTL %d.%d.%d",
                     g_version_major,
                     g_version_minor,
                     g_version_rev);
            basic_string_set(&tmp, buf);
            sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y  = sz.y;
            basic_string_destroy(&tmp);
        }

        if (settings->fps == i) {
            // if we're time warping, the calculated FPS will include dummy frames that aren't
            // actually rendered
            if (TimeWarp_feature.enabled && gs.timeWarpActive && gs.warpFactor > 1) {
                snprintf(buf, sizeof(buf), "FPS: %d (%d)", gs.lastRender1s, gs.lastFrame1s);
            } else {
                snprintf(buf, sizeof(buf), "FPS: %d", gs.lastRender1s);
            }
            basic_string_set(&tmp, buf);
            sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y  = sz.y;
            basic_string_destroy(&tmp);
        }
    }
}

// ---- Patching ----------------

Patch* InfoBlock_patches[] = { &patch_FTLButton_OnRender,
                               &patch_TextLibrary_GetText,
                               &patch_CApp_OnRender,
                               0 };

FeatureSettingsSpec InfoBlock_spec = {
    .size = sizeof(InfoBlockSettings),
    .ent  = { { .name = "ssver", .type = CF_INT, .off = offsetof(InfoBlockSettings, ssver) },
             { .name = "ftlver", .type = CF_INT, .off = offsetof(InfoBlockSettings, ftlver) },
             { .name = "fps", .type = CF_INT, .off = offsetof(InfoBlockSettings, fps) },
             { 0 } },
};

SubspaceFeature InfoBlock_feature = {
    .name            = "InfoBlock",
    .settingsspec    = &InfoBlock_spec,
    .requiredPatches = InfoBlock_patches,
    .requiredSymbols = { &SYM(freetype_easy_printRightAlign),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(version_major),
                        &SYM(version_minor),
                        &SYM(version_rev),
                        0 }
};
