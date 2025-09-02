#include "control.h"

#include "ftl/stdlib.h"

#include "feature/feature.h"
#include "feature/screenshot.h"
#include "feature/timewarp.h"
#include "ftl/capp.h"
#include "ftl/globals.h"
#include "ftl/graphics/colors.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "ftl/misc.h"
#include "ftl/starmap.h"
#include "ftl/struct.h"
#include "ftl/textlibrary.h"
#include "ftl/worldmanager.h"
#include "patch/patchlist.h"
#include "infoblock.h"

#define INFOBLOCK_FONT 6

void infoBlockRender(void)
{
    InfoBlockSettings* settings = (InfoBlockSettings*)InfoBlock_feature.settings;

    if (screenshotHideInfoBlock())
        return;

    char buf[128];
    basic_string tmp;
    Pointf sz;
    float x    = 1280 - 5;
    float y    = 5;
    float colw = 0;
    int nrows  = 0;

    CSurface_GL_SetColor(COLOR_WHITE);

    for (int i = 0; i < 4; i++) {
        if (settings->ssver == i) {
#ifdef _DEBUG
            snprintf(buf, sizeof(buf), "Subspace %s (DEBUG BUILD)", subspace_version_str);
#else
            snprintf(buf, sizeof(buf), "Subspace %s", subspace_version_str);
#endif
            basic_string_set(&tmp, buf);
            sz   = easy_measurePrintLines(INFOBLOCK_FONT, 0, 0, 999, &tmp);
            colw = max(colw, sz.x);
            sz   = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y    = sz.y;
            nrows++;
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
            sz   = easy_measurePrintLines(INFOBLOCK_FONT, 0, 0, 999, &tmp);
            colw = max(colw, sz.x);
            sz   = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y    = sz.y;
            nrows++;
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
            sz   = easy_measurePrintLines(INFOBLOCK_FONT, 0, 0, 999, &tmp);
            colw = max(colw, sz.x);
            sz   = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y    = sz.y;
            nrows++;
            basic_string_destroy(&tmp);
        }

        if (settings->sector == i) {
            WorldManager* world = CApp_world(theApp);
            int secnum          = WorldManager_worldLevel(world) + 1;
            TextString* secname =
                Sector_description_shortName(StarMap_currentSector(WorldManager_starMap(world)));

            basic_string_reset(&tmp);
            GetText(&secname->data, &tmp);

            snprintf(buf, sizeof(buf), "%d - %s", secnum, tmp.buf);
            basic_string_destroy(&tmp);

            basic_string_set(&tmp, buf);
            sz   = easy_measurePrintLines(INFOBLOCK_FONT, 0, 0, 999, &tmp);
            colw = max(colw, sz.x);
            sz   = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
            y    = sz.y;
            nrows++;
            basic_string_destroy(&tmp);
        }

        if (nrows % 3 == 0) {
            // only 3 rows can fit above the target box; if we have more, need to spill over into a
            // new column
            x -= colw + 20;
            colw = 0;
            y    = 5;
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
             { .name = "sector", .type = CF_INT, .off = offsetof(InfoBlockSettings, sector) },
             { 0 } },
};

SubspaceFeature InfoBlock_feature = {
    .name            = "InfoBlock",
    .settingsspec    = &InfoBlock_spec,
    .requiredPatches = InfoBlock_patches,
    .requiredSymbols = { &SYM(freetype_easy_printRightAlign),
                        &SYM(freetype_easy_measurePrintLines),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(version_major),
                        &SYM(version_minor),
                        &SYM(version_rev),
                        &SYM(CApp_world_offset),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(WorldManager_starMap_worldLevel_offset),
                        &SYM(Sector_description_shortName_offset),
                        &SYM(StarMap_currentSector_offset),
                        0 }
};
