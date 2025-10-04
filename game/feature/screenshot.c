#include "ftl/stdlib.h"

#include "feature/feature.h"
#include "ftl/blueprintmanager.h"
#include "ftl/bossship.h"
#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/filehelper.h"
#include "ftl/globals.h"
#include "ftl/graphics/csurface.h"
#include "ftl/misc.h"
#include "ftl/scorekeeper.h"
#include "ftl/shipmanager.h"
#include "ftl/sil.h"
#include "ftl/soundcontrol.h"
#include "ftl/starmap.h"
#include "ftl/worldmanager.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"
#include "util/lcg.h"
#include "osdep.h"
#include "ospatchlist.h"
#include "screenshot.h"

static bool currentScreenshotAuto;
static lock_t soundlock;
static bool soundlock_init;
static bool screenshotSound;

static bool playerDestroyed;
static bool enemyDestroyed;

typedef struct SSInfo {
    char* fn;
    int w;
    int h;
    uint8_t* buf;

    // metadata to send
    char* shiptype;
    char* shipname;
    char* sectortype;
    int difficulty;
    int sector;
    int beacon;
    int seed;
    bool automatic;
} SSInfo;

static int ss_write_thread(void* data)
{
    SSInfo* ss = (SSInfo*)data;

    // flip the image, remote alpha channel

    size_t bsz         = ss->w * ss->h * 3;
    uint8_t* processed = malloc(bsz);

    for (int y = 0; y < ss->h; y++) {
        uint8_t* srcrow  = &ss->buf[y * ss->w * 4];
        uint8_t* destrow = &processed[(ss->h - y - 1) * ss->w * 3];
        for (int x = 0; x < ss->w; x++) {
            destrow[x * 3]     = srcrow[x * 4];
            destrow[x * 3 + 1] = srcrow[x * 4 + 1];
            destrow[x * 3 + 2] = srcrow[x * 4 + 2];
        }
    }
    osWriteFile(ss->fn, processed, bsz);
    free(processed);

    ControlMsg* msg = controlNewMsg("Screenshot", 14);
    controlMsgBool(msg, 0, "auto", ss->automatic);
    controlMsgStr(msg, 1, "filename", ss->fn);
    controlMsgInt(msg, 2, "w", ss->w);
    controlMsgInt(msg, 3, "h", ss->h);
    controlMsgStr(msg, 4, "ship", ss->shiptype);
    controlMsgStr(msg, 5, "name", ss->shipname);
    controlMsgInt(msg, 6, "sector", ss->sector);
    controlMsgStr(msg, 7, "sectortype", ss->sectortype);
    controlMsgInt(msg, 8, "beacon", ss->beacon);
    controlMsgUInt(msg, 9, "seed", ss->seed);
    controlMsgInt(msg, 10, "difficulty", ss->difficulty);
    controlMsgInt(msg, 11, "major", g_version_major);
    controlMsgInt(msg, 12, "minor", g_version_minor);
    controlMsgInt(msg, 13, "rev", g_version_rev);
    controlClientQueue(msg);

    lock_acq(&soundlock);
    screenshotSound = true;
    lock_rel(&soundlock);

    free(ss->shiptype);
    free(ss->shipname);
    free(ss->sectortype);
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

// metadata should be captured in the main thread
static void screenshotMetadata(SSInfo* ss)
{
    WorldManager* world      = CApp_world(theApp);
    CompleteShip* playerShip = world ? WorldManager_playerShip(world) : NULL;
    StarMap* map             = world ? WorldManager_starMap(world) : NULL;
    ShipManager* smgr        = playerShip ? CompleteShip_shipManager(playerShip) : NULL;
    ShipBlueprint* sbp       = smgr ? ShipManager_myBlueprint(smgr) : NULL;
    basic_string* shipType   = sbp ? ShipBlueprint_blueprintName(sbp) : NULL;
    TextString* shipName     = sbp ? ShipBlueprint_name(sbp) : NULL;
    Sector* sector           = map ? StarMap_currentSector(map) : NULL;
    StatTracker* stats       = ScoreKeeper_stats(SKeeper);

    if (shipType && shipName && sector && stats) {
        ss->shiptype   = strdup(shipType->buf);
        ss->shipname   = strdup(shipName->data.buf);
        ss->sectortype = strdup(Sector_description_type(sector)->buf);
        ss->difficulty = g_Settings_difficulty;
        ss->sector     = StarMap_worldLevel(map);
        ss->beacon     = stats[1].current;
        ss->seed       = StarMap_sectorMapSeed(map);
    } else {
        ss->shiptype      = malloc(1);
        ss->shiptype[0]   = 0;
        ss->shipname      = malloc(1);
        ss->shipname[0]   = 0;
        ss->sectortype    = malloc(1);
        ss->sectortype[0] = 0;
        ss->difficulty    = 0;
        ss->sector        = 0;
        ss->beacon        = 0;
        ss->seed          = 0;
    }
}

void saveScreenshotFramebuf(int* fb)
{
    if (!fb)
        return;

    int tex    = framebuffer_get_texture(*fb);
    void* data = texture_lock_readonly(tex);
    if (!data)
        return;

    SSInfo* ss = malloc(sizeof(SSInfo));
    ss->fn     = makeScreenshotFilename();
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

    ss->automatic = currentScreenshotAuto;
    screenshotMetadata(ss);
    osStartThread(ss_write_thread, ss);
}

void saveScreenshotFallback(void)
{
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

    ss->automatic = currentScreenshotAuto;
    screenshotMetadata(ss);
    osStartThread(ss_write_thread, ss);
}

void renderScreenshot(CApp* app, bool automatic)
{
    gs.renderingScreenshot = true;
    currentScreenshotAuto  = automatic;
    CApp_OnRender(app);
    currentScreenshotAuto  = false;
    gs.renderingScreenshot = false;
}

void screenshotCheckSound(void)
{
    ScreenshotSettings* settings = Screenshot_feature.settings;

    if (!soundlock_init) {
        lock_init(&soundlock);
        soundlock_init = true;
    }

    lock_acq(&soundlock);
    bool playsound  = screenshotSound;
    screenshotSound = false;
    lock_rel(&soundlock);

    if (playsound && settings->sound) {
        // use a unique mix of 2 select sounds

        basic_string temp;
        basic_string_set(&temp, "hoverBeep");
        SoundControl_PlaySoundMix(SCSounds, &temp, -1, false);
        basic_string_destroy(&temp);

        basic_string_set(&temp, "powerUpFail");
        SoundControl_PlaySoundMix(SCSounds, &temp, -1, false);
        basic_string_destroy(&temp);
    }
}

void screenshotCheckDestroyed(void)
{
    WorldManager* world  = CApp_world(theApp);
    CompleteShip* pcship = world ? WorldManager_playerShip(world) : NULL;
    ShipManager* pship   = pcship ? CompleteShip_shipManager(pcship) : NULL;
    if (pship && ShipManager_bDestroyed(pship)) {
        if (!playerDestroyed && screenshotAuto(SSEvent_Destroyed)) {
            playerDestroyed      = true;
            gs.screenshotNowAuto = true;
        }
    } else {
        playerDestroyed = false;
    }

    CompleteShip* ecship = pcship ? CompleteShip_enemyShip(pcship) : NULL;
    ShipManager* eship   = ecship ? CompleteShip_shipManager(ecship) : NULL;
    if (eship && ShipManager_bDestroyed(eship)) {
        if (!enemyDestroyed) {
            if (screenshotAuto(SSEvent_WinFight)) {
                enemyDestroyed       = true;
                gs.screenshotNowAuto = true;
            } else {
                // otherwise maybe this is a boss stage, check the specific events for those
                BossShip* bship = WorldManager_bossShip(world);
                if ((CompleteShip*)bship == ecship) {
                    if ((BossShip_currentStage(bship) == 1 && screenshotAuto(SSEvent_RFS1)) ||
                        (BossShip_currentStage(bship) == 2 && screenshotAuto(SSEvent_RFS2)) ||
                        (BossShip_currentStage(bship) == 3 && screenshotAuto(SSEvent_RFS3))) {
                        enemyDestroyed       = true;
                        gs.screenshotNowAuto = true;
                    }
                }
            }
        }
    } else {
        enemyDestroyed = false;
    }
}

bool screenshotHideMouse(void)
{
    ScreenshotSettings* settings = Screenshot_feature.settings;
    return gs.renderingScreenshot && settings->hidemouse;
}

bool screenshotHidePause(void)
{
    ScreenshotSettings* settings = Screenshot_feature.settings;
    return gs.renderingScreenshot && settings->hidepause;
}

bool screenshotHideInfoBlock(void)
{
    ScreenshotSettings* settings = Screenshot_feature.settings;
    return gs.renderingScreenshot && settings->hideinfoblock;
}

bool screenshotUseFramebuf(void)
{
    // framebuffer grabbing is broken in D3d, use the fallback
    return CApp_useFrameBuffer(theApp) && !osIsUsingDirect3D();
}

bool screenshotAuto(int event)
{
    if (!Screenshot_feature.enabled)
        return false;

    ScreenshotSettings* settings = Screenshot_feature.settings;
    return !!(settings->events & event);
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

FeatureSettingsSpec Screenshot_spec = {
    .size = sizeof(ScreenshotSettings),
    .ent  = { { .name = "sound", .type = CF_BOOL, .off = offsetof(ScreenshotSettings, sound) },
             { .name = "hidemouse",
                .type = CF_BOOL,
                .off  = offsetof(ScreenshotSettings, hidemouse) },
             { .name = "hidepause",
                .type = CF_BOOL,
                .off  = offsetof(ScreenshotSettings, hidepause) },
             { .name = "hideinfoblock",
                .type = CF_BOOL,
                .off  = offsetof(ScreenshotSettings, hideinfoblock) },
             { .name = "events", .type = CF_INT, .off = offsetof(ScreenshotSettings, events) },
             { 0 } }
};

// ---- Patching ----------------

Patch* Screenshot_patches[] = {
    &patch_CSurface_FinishFrame,
    &patch_CSurface_GL_BlitFrameBuffer,
    &patch_CApp_OnRender,
    &patch_MouseControl_OnRender,
    &patch_CommandGui_RenderPause,
    &patch_AchievementTracker_OnLoop,
    &patch_GameOver_OnLoop,
#ifdef WIN32
    &patch_SILTextureLock,
#endif
    0
};

SubspaceFeature Screenshot_feature = {
    .name            = "Screenshot",
    .keybinds        = Screenshot_keybinds,
    .requiredPatches = Screenshot_patches,
    .settingsspec    = &Screenshot_spec,
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
                        &SYM(WorldManager_playerShip_offset),
                        &SYM(WorldManager_starMap_offset),
                        &SYM(WorldManager_bossShip_offset),
                        &SYM(BossShip_currentStage_offset),
                        &SYM(CompleteShip_shipManager_offset),
                        &SYM(CompleteShip_enemyShip_offset),
                        &SYM(ShipManager_myBlueprint_offset),
                        &SYM(Settings_difficulty),
                        &SYM(SoundControl_Sounds),
                        &SYM(SoundControl_PlaySoundMix),
#ifdef WIN32
                        &SYM(CApp_useDirect3D_offset),
#endif
                        0 }
};
