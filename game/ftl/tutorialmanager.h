#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct TutorialManager TutorialManager;

// FTL functions & hooks below

DECLSYM(TutorialManager_Tutorial);
#define Tutorial ((TutorialManager*)symAddr(ftlbase, TutorialManager_Tutorial))

typedef bool (*FUNCTYPE(TutorialManager_Running))(TutorialManager* self);
DECLFUNC(TutorialManager_Running);
#define TutorialManager_Running(self) FCALL(ftlbase, TutorialManager_Running, self)