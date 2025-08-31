#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct GameOver GameOver;
typedef struct CreditScreen CreditScreen;

extern DisasmTrace GameOver_OnLoop_trace;

// FTL functions & wrappers below

typedef int (*FUNCTYPE(GameOver_SetVictory))(GameOver* self, bool victory, basic_string *shipName, void *crew);
DECLFUNC(GameOver_SetVictory);

typedef void (*FUNCTYPE(GameOver_OpenText))(GameOver* self, basic_string *text);
DECLFUNC(GameOver_OpenText);

typedef void (*FUNCTYPE(GameOver_OnLoop))(GameOver* self);
DECLFUNC(GameOver_OnLoop);

DECLSYM(GameOver_bVictory_offset);
#define GameOver_bVictory(gmo) MEMBER(ftlbase, GameOver, gmo, bool, bVictory)
DECLSYM(GameOver_bShowingCredits_offset);
#define GameOver_bShowingCredits(gmo) MEMBER(ftlbase, GameOver, gmo, bool, bShowingCredits)
DECLSYM(GameOver_credits_offset);
#define GameOver_credits(gmo) (&MEMBER(ftlbase, GameOver, gmo, CreditScreen, credits))
