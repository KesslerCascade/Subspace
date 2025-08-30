#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct AchievementTracker AchievementTracker;

extern DisasmTrace AchievementTracker_OnLoop_trace;

// FTL functions & wrappers below

DECLSYM(AchievementTracker_Tracker);
#define AchTracker ((CFPS*)symAddr(ftlbase, AchievementTracker_Tracker))

typedef void (*FUNCTYPE(AchievementTracker_OnLoop))(AchievementTracker* self);
DECLFUNC(AchievementTracker_OnLoop);

typedef void (*FUNCTYPE(AchievementTracker_ResetFlags))(AchievementTracker* self);
DECLFUNC(AchievementTracker_ResetFlags);

typedef void (*FUNCTYPE(AchievementTracker_SetAchievement))(AchievementTracker* self,
                                                            basic_string* achievement, bool noPopup,
                                                            bool sendToServer);
DECLFUNC(AchievementTracker_SetAchievement);
