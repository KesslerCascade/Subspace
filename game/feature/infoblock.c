#include "ftl/stdlib.h"

#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/graphics/colors.h"
#include "ftl/graphics/csurface.h"
#include "ftl/graphics/freetype.h"
#include "ftl/struct.h"
#include "ftl/misc.h"
#include "patch/seq/seq_infoblock.h"
#include "infoblock.h"

#define INFOBLOCK_FONT 6

void infoBlockRender(void)
{
    char buf[128];
    basic_string tmp;
    Pointf sz;
    float x = 1280 - 5;
    float y = 5;

    CSurface_GL_SetColor(COLOR_WHITE);
    #ifdef _DEBUG
    snprintf(buf, sizeof(buf), "Subspace %s (DEBUG BUILD)", subspace_version_str);
    #else
    snprintf(buf, sizeof(buf), "Subspace %s", subspace_version_str);
    #endif
    basic_string_set(&tmp, buf);
    sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
    y = sz.y;
    basic_string_destroy(&tmp);

    snprintf(buf, sizeof(buf), "FTL %d.%d.%d", g_version_major, g_version_minor, g_version_rev);
    basic_string_set(&tmp, buf);
    sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
    y = sz.y;
    basic_string_destroy(&tmp);

    snprintf(buf, sizeof(buf), "FPS: %d", CFPS_fps(FPSControl));
    basic_string_set(&tmp, buf);
    sz = easy_printRightAlign(INFOBLOCK_FONT, x, y, &tmp);
    y = sz.y;
    basic_string_destroy(&tmp);
}

// ---- Patching ----------------

static bool infoBlock_Patch(SubspaceFeature* feat, void* settings, PatchState* ps)
{
    return patchApplySeq(ps, InfoBlockPatches);
}

SubspaceFeature InfoBlock_feature = {
    .patch           = infoBlock_Patch,
    .requiredSymbols = { &SYM(freetype_easy_printRightAlign),
                        &SYM(CSurface_GL_SetColor),
                        &SYM(version_major),
                        &SYM(version_minor),
                        &SYM(version_rev),
                        &SYM(CFPS_FPSControl),
                        &SYM(CFPS_fps_offset),
                        0 }
};
