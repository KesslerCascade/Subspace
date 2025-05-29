#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ScoreKeeper ScoreKeeper;

// FTL functions & wrappers below

extern DisasmTrace ScoreKeeper_Save_trace;

DECLSYM(ScoreKeeper_Keeper);
#define SKeeper ((ScoreKeeper*)symAddr(ftlbase, ScoreKeeper_Keeper))

typedef void (*FUNCTYPE(ScoreKeeper_Save))(ScoreKeeper* self, bool newHighscore);
DECLFUNC(ScoreKeeper_Save);

typedef void (*FUNCTYPE(ScoreKeeper_SetVictory))(ScoreKeeper* self, bool victory);
DECLFUNC(ScoreKeeper_SetVictory);

typedef void (*FUNCTYPE(ScoreKeeper_SetSector))(ScoreKeeper* self, int sector);
DECLFUNC(ScoreKeeper_SetSector);

typedef void (*FUNCTYPE(ScoreKeeper_Reset))(ScoreKeeper* self);
DECLFUNC(ScoreKeeper_Reset);