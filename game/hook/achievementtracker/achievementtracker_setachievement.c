#include "control/runlog.h"
#include "ftl/achievementtracker.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

int subspace_AchievementTracker_SetAchievement_pre(AchievementTracker* self,
                                                   basic_string* achievement, bool noPopup,
                                                   bool sendToServer)
{
    if (gs.practiceMode)
        return 0;   // no achievements for you! for one year!

    return 1;
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        AchievementTracker_SetAchievement,
                        subspace_AchievementTracker_SetAchievement_pre,
                        NULL);
}

Patch patch_AchievementTracker_SetAchievement = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(AchievementTracker_SetAchievement), 0 }
};
