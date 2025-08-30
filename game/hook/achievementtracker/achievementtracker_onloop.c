#include "control/runlog.h"
#include "feature/screenshot.h"
#include "ftl/achievementtracker.h"
#include "ftl/animationtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static bool animrunning;

int subspace_AchievementTracker_OnLoop_pre(AchievementTracker* self)
{
    AnimationTracker* popup = AchievementTracker_achievementPopup(self);
    if (popup && Screenshot_feature.enabled) {
        animrunning = AnimationTracker_running(popup);
    }

    return 1;
}

void subspace_AchievementTracker_OnLoop_post(AchievementTracker* self)
{
    AnimationTracker* popup = AchievementTracker_achievementPopup(self);
    if (popup && Screenshot_feature.enabled && screenshotAuto(SSEvent_Ach)) {
        // did we pop up a new achievement this frame?
        if (!animrunning && AnimationTracker_running(popup))
            gs.screenshotNowAuto = true;
    }
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        AchievementTracker_OnLoop,
                        subspace_AchievementTracker_OnLoop_pre,
                        subspace_AchievementTracker_OnLoop_post);
}

Patch patch_AchievementTracker_OnLoop = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(AchievementTracker_OnLoop),
                        &SYM(AchievementTracker_achievementPopup_offset),
                        &SYM(AnimationTracker_running_offset),
                        0 }
};
