#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct AchievementTracker AchievementTracker;

// FTL functions & wrappers below

DECLSYM(AchievementTracker_Tracker);
#define AchTracker ((CFPS*)symAddr(ftlbase, AchievementTracker_Tracker))

typedef void (*FUNCTYPE(AchievementTracker_OnLoop))(AchievementTracker* self);
DECLFUNC(AchievementTracker_OnLoop);
