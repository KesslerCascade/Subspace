#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace AchievementTracker_OnLoop_trace;
extern DisasmTrace AchievementTracker_OnLoop_trace_2;

// FTL functions & wrappers below

DECLSYM(AchievementTracker_Tracker);
#define AchTracker ((CFPS*)symAddr(ftlbase, AchievementTracker_Tracker))

typedef void (*FUNCTYPE(AchievementTracker_OnLoop))(AchievementTracker* self);
DECLFUNC(AchievementTracker_OnLoop);

typedef void (*FUNCTYPE(AchievementTracker_SetFlag))(AchievementTracker* self, string* flag);
DECLFUNC(AchievementTracker_SetFlag);

typedef void (*FUNCTYPE(AchievementTracker_ResetFlags))(AchievementTracker* self);
DECLFUNC(AchievementTracker_ResetFlags);

typedef void (*FUNCTYPE(AchievementTracker_SetAchievement))(AchievementTracker* self,
                                                            basic_string* achievement, bool noPopup,
                                                            bool sendToServer);
DECLFUNC(AchievementTracker_SetAchievement);

DECLSYM(AchievementTracker_achievementPopup_offset);
#define AchievementTracker_achievementPopup(atracker) \
    (&MEMBER(ftlbase, AchievementTracker, atracker, AnimationTracker, achievementPopup))
