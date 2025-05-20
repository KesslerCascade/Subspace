#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ScoreKeeper ScoreKeeper;

// FTL functions & wrappers below

typedef void (*FUNCTYPE(ScoreKeeper_Save))(ScoreKeeper* self, bool newHighscore);
DECLFUNC(ScoreKeeper_Save);
