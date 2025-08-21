#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ScoreKeeper ScoreKeeper;

typedef struct StatTracker {
    int maximum;
    int total;
    int current;
    basic_string desc_id;
    int sector;
} StatTracker;

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

typedef void (*FUNCTYPE(ScoreKeeper_AddCrew))(ScoreKeeper* self);
DECLFUNC(ScoreKeeper_AddCrew);

typedef void (*FUNCTYPE(ScoreKeeper_AddDefeatedShips))(ScoreKeeper* self);
DECLFUNC(ScoreKeeper_AddDefeatedShips);

typedef void (*FUNCTYPE(ScoreKeeper_AddExploredLocations))(ScoreKeeper* self);
DECLFUNC(ScoreKeeper_AddExploredLocations);

typedef void (*FUNCTYPE(ScoreKeeper_AddScrapCollected))(ScoreKeeper* self, int amount);
DECLFUNC(ScoreKeeper_AddScrapCollected);

typedef void (*FUNCTYPE(ScoreKeeper_UnlockShip))(ScoreKeeper* self, int shipId, int layout,
                                                 bool save, bool hidePopup);
DECLFUNC(ScoreKeeper_UnlockShip);

// we know this is always at offset 0 in all versions
#define ScoreKeeper_stats(keeper) ((StatTracker*)(keeper))