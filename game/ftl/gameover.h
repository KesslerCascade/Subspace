#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct GameOver GameOver;

// FTL functions & wrappers below

typedef int (*FUNCTYPE(GameOver_SetVictory))(GameOver* self, bool victory, basic_string *shipName, void *crew);
DECLFUNC(GameOver_SetVictory);

typedef void (*FUNCTYPE(GameOver_OpenText))(GameOver* self, basic_string *text);
DECLFUNC(GameOver_OpenText);
