#include "feature/screenshot.h"
#include "ftl/graphics/csurface.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

// ---- Hooks ----------------

int subspace_CSurface_GL_BlitFrameBuffer_pre(int* fb, float size_x, float size_y, bool aa)
{
    if (Screenshot_feature.enabled && gs.renderingScreenshot) {
        if (screenshotUseFramebuf()) {
            saveScreenshotFramebuf(fb);   // save pre-scaled buffer (1280x720)
            return 0;                     // don't actually blit it
        }
    }

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        CSurface_GL_BlitFrameBuffer,
                        subspace_CSurface_GL_BlitFrameBuffer_pre,
                        NULL);
}

Patch patch_CSurface_GL_BlitFrameBuffer = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(CSurface_GL_BlitFrameBuffer), 0 }
};
