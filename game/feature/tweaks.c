#include "tweaks.h"
#include <stddef.h>
#include "feature/feature.h"
#include "feature/timewarp.h"
#include "ftl/globals.h"
#include "input/keybinds.h"
#include "patch/patchlist.h"

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

Patch* Tweaks_patches[] = { &patch_CFPS_OnLoop, &patch_CFPS_TargetFrameTime, 0 };

FeatureSettingsSpec Tweaks_spec = {
    .size = sizeof(TweaksSettings),
    .ent  = { { .name = "targetfps", .type = CF_INT, .off = offsetof(TweaksSettings, targetfps) },
             { 0 } }
};

SubspaceFeature Tweaks_feature = {
    .name            = "Tweaks",
    .enable          = tweaks_Enable,
    .settingsspec    = &Tweaks_spec,
    .requiredPatches = Tweaks_patches,
    .requiredSymbols = { 0 }
};
