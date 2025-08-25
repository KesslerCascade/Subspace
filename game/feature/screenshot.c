#include "ftl/stdlib.h"

#include "feature/feature.h"
#include "ftl/capp.h"
#include "ftl/filehelper.h"
#include "ftl/graphics/csurface.h"
#include "ftl/sil.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"
#include "util/lcg.h"
#include "osdep.h"
#include "ospatchlist.h"
#include "screenshot.h"

typedef struct SSInfo {
    char* fn;
    int w;
    int h;
    uint8_t* buf;
} SSInfo;

static int ss_write_thread(void* data)
{
    SSInfo* ss = (SSInfo*)data;

    // flip the image, and set alpha channel to 255

    size_t bsz         = ss->w * ss->h * 4;
    uint8_t* processed = malloc(bsz);

    for (int y = 0; y < ss->h; y++) {
        uint8_t* srcrow  = &ss->buf[y * ss->w * 4];
        uint8_t* destrow = &processed[(ss->h - y - 1) * ss->w * 4];
        for (int x = 0; x < ss->w; x++) {
            destrow[x * 4]     = srcrow[x * 4];
            destrow[x * 4 + 1] = srcrow[x * 4 + 1];
            destrow[x * 4 + 2] = srcrow[x * 4 + 2];
            destrow[x * 4 + 3] = 0xff;
        }
    }
    osWriteFile(ss->fn, processed, bsz);
    free(processed);

    free(ss->fn);
    free(ss->buf);
    free(ss);

    return 0;
}

static char* makeScreenshotFilename(void)
{
    char* ret = NULL;

    basic_string sfile;
    basic_string_reset(&sfile);
    subspace_FileHelper_getUserFolder(&sfile);

    char* tempfn = malloc(1024);
    ret          = malloc(1024);
    strcpy(tempfn, sfile.buf);
    size_t len = strlen(sfile.buf);
    xsnprintf(tempfn + len, 1024 - len, "sshot-%08d.raw", lcg_random() % 100000000);
    if (!osAbsolutePathUTF8(tempfn, ret, 1024)) {
        free(tempfn);
        free(ret);
        basic_string_destroy(&sfile);
        return NULL;
    }

    basic_string_destroy(&sfile);
    free(tempfn);
    return ret;
}

void saveScreenshotFramebuf(int* fb)
{
    if (!fb)
        return;

    int tex    = framebuffer_get_texture(*fb);
    void* data = texture_lock_readonly(tex);
    if (!data)
        return;

    SSInfo* ss   = malloc(sizeof(SSInfo));
    ss->fn       = makeScreenshotFilename();
    if (!ss->fn) {
        free(ss);
        return;
    }

    ss->w        = texture_width(tex);
    ss->h        = texture_height(tex);
    size_t dsize = ss->w * ss->h * 4;
    ss->buf      = malloc(dsize);
    memcpy(ss->buf, data, dsize);
    texture_unlock(tex);

    osStartThread(ss_write_thread, ss);
}

void saveScreenshotFallback(void)
{
    // TODO: get actual size...

    SSInfo* ss = malloc(sizeof(SSInfo));
    ss->fn     = makeScreenshotFilename();
    if (!ss->fn) {
        free(ss);
        return;
    }

    int modx = CApp_modifier_x(theApp);
    int mody = CApp_modifier_y(theApp);
    int barx = CApp_x_bar(theApp);
    int bary = CApp_y_bar(theApp);

    ss->w        = CApp_screen_x(theApp) - modx * 2 - barx * 2;
    ss->h        = CApp_screen_y(theApp) - mody * 2 - bary * 2;
    size_t dsize = ss->w * ss->h * 4;
    ss->buf      = malloc(dsize);
    sys_graphics_read_pixels(modx + barx, mody + bary, ss->w, ss->h, ss->w, ss->buf);

    osStartThread(ss_write_thread, ss);
}

void renderScreenshot(CApp* app)
{
    gs.renderingScreenshot = true;
    CApp_OnRender(app);
    gs.renderingScreenshot = false;
}

bool screenshotHideMouse(void)
{
    return gs.renderingScreenshot;
}

bool screenshotHidePause(void)
{
    return gs.renderingScreenshot;
}

bool screenshotUseFramebuf(void)
{
    // framebuffer grabbing is broken in D3d, use the fallback
    return CApp_useFrameBuffer(theApp) && !osIsUsingDirect3D();
}

// ---- Keybinds ----------------

static void screenshot_take_cb(int key, int flags)
{
    gs.screenshotNow = true;
}

static KeyBind Screenshot_keybinds[] = {
    { .name = "screenshot_take", .context = KB_CTX_GAME, .func = screenshot_take_cb },
    { 0 }
};

// ---- Patching ----------------

Patch* Screenshot_patches[] = {
    &patch_CSurface_FinishFrame,
    &patch_CSurface_GL_BlitFrameBuffer,
    &patch_CApp_OnRender,
    &patch_MouseControl_OnRender,
    &patch_CommandGui_RenderPause,
#ifdef WIN32
    &patch_SILTextureLock,
#endif
    0
};

SubspaceFeature Screenshot_feature = {
    .name            = "Screenshot",
    .keybinds        = Screenshot_keybinds,
    .requiredPatches = Screenshot_patches,
    .requiredSymbols = { &SYM(texture_lock_readonly),
                        &SYM(texture_width),
                        &SYM(texture_height),
                        &SYM(texture_unlock),
                        &SYM(framebuffer_get_texture),
                        &SYM(CApp_framebuffer_offset),
                        &SYM(CApp_useFrameBuffer_offset),
                        &SYM(CApp_screen_x_offset),
                        &SYM(CApp_screen_y_offset),
                        &SYM(CApp_x_bar_offset),
                        &SYM(CApp_y_bar_offset),
                        &SYM(CApp_modifier_x_offset),
                        &SYM(CApp_modifier_y_offset),
                        &SYM(sys_graphics_read_pixels),
#ifdef WIN32
                        &SYM(CApp_useDirect3D_offset),
#endif
                        0 }
};
