#pragma once
#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef void (*FUNCTYPE(AnimationTracker_Update))(AnimationTracker* self);
DECLFUNC(AnimationTracker_Update);

DECLSYM(AnimationTracker_running_offset);
#define AnimationTracker_running(atracker) MEMBER(ftlbase, AnimationTracker, atracker, bool, running)
DECLSYM(AnimationTracker_done_offset);
#define AnimationTracker_done(atracker) MEMBER(ftlbase, AnimationTracker, atracker, bool, done)