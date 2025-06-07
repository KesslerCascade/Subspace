#include "ftl/stdlib.h"

#include <stddef.h>
#include "feature/feature.h"
#include "feature/timewarp.h"
#include "ftl/filehelper.h"
#include "ftl/globals.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"
#include "tweaks.h"

#include "minicrt.h"

int tweaksGetTargetFPS()
{
    TweaksSettings* settings = Tweaks_feature.settings;

    return settings->targetfps;
}

void tweaksSetFrameTime()
{
    TweaksSettings* settings = Tweaks_feature.settings;

    // don't touch frame time during time warp; the TimeWarp feature will manage that because of frameskip!
    if (gs.timeWarpActive)
        return;

    g_TargetFrameTimeMS = (double)1000. / (double)settings->targetfps;
}

bool tweaksPreserveLoad()
{
    TweaksSettings* settings = Tweaks_feature.settings;
    return settings->preserveload;
}

void tweaksPrepareSave()
{
    TweaksSettings* settings = Tweaks_feature.settings;
    if (settings->savecompat) {
        basic_string sfile;
        basic_string_reset(&sfile);
        FileHelper_getSaveFile(&sfile);

        if (gs.saveFileOverride)
            free(gs.saveFileOverride);   // shouldn't be possible? but don't leak anyway

        gs.saveFileOverride = malloc(sfile.len + 14);
        memcpy(gs.saveFileOverride, sfile.buf, sfile.len);
        memcpy(gs.saveFileOverride + sfile.len, ".subspace-new", 14);
        basic_string_destroy(&sfile);
    }
}

void tweaksFinalizeSave()
{
    // NOTE: We do NOT check settings here; once that check is performed in tweaksPrepareSave, we're
    // committed
    if (gs.saveFileOverride) {
        basic_string sfile;
        basic_string_reset(&sfile);
        FileHelper_getSaveFile(&sfile);

        basic_string nfile;
        basic_string_set(&nfile, gs.saveFileOverride);

        // double check that the overriden file was actually created
        if (FileHelper_fileExists(&nfile)) {
            // delete save file, rename newly created save into place.
            // interestingly, the game already does this for ae_prof.sav, just not continue.sav
            FileHelper_deleteFile(&sfile);
            FileHelper_renameFile(&nfile, &sfile);
        }

        basic_string_destroy(&sfile);
        basic_string_destroy(&nfile);

        free(gs.saveFileOverride);
        gs.saveFileOverride = NULL;
    }
}

// ---- Patching ----------------

static bool tweaks_Enable(SubspaceFeature* feat, void* settings, bool enabled)
{
    gs.tweaksOverrideFPS = enabled;
    if (!enabled) {
        if (gs.timeWarpActive)
            timeWarpEnd();      // need to change the frame time back but can't do it with time warp active
        g_TargetFrameTimeMS = g_DefaultFrameTimeMS;
    }

    return enabled;
}

Patch* Tweaks_patches[] = {
    &patch_CFPS_OnLoop,
    &patch_CFPS_TargetFrameTime,
    &patch_FileHelper_deleteFile,
    &patch_FileHelper_getSaveFile,
    &patch_WorldManager_LoadGame,
    &patch_WorldManager_SaveGame,
    0
};

FeatureSettingsSpec Tweaks_spec = {
    .size = sizeof(TweaksSettings),
    .ent  = { { .name = "targetfps", .type = CF_INT, .off = offsetof(TweaksSettings, targetfps) },
             { .name = "preserveload",
                .type = CF_BOOL,
                .off  = offsetof(TweaksSettings, preserveload) },
             { .name = "savecompat", .type = CF_BOOL, .off = offsetof(TweaksSettings, savecompat) },
             { 0 } }
};

SubspaceFeature Tweaks_feature = {
    .name            = "Tweaks",
    .enable          = tweaks_Enable,
    .settingsspec    = &Tweaks_spec,
    .requiredPatches = Tweaks_patches,
    .requiredSymbols = { &SYM(FileHelper_fileExists), &SYM(FileHelper_renameFile), 0 }
};
