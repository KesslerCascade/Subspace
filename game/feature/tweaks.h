#pragma once
#include "feature.h"
#include "subspacegame.h"

typedef struct SoundControl SoundControl;

typedef struct TweaksSettings {
    int targetfps;
    bool preserveload;
    bool savecompat;
    bool creditsmusic;
} TweaksSettings;

int tweaksGetTargetFPS();
void tweaksSetFrameTime();
bool tweaksPreserveLoad();
void tweaksPrepareSave();
void tweaksFinalizeSave();
bool tweaksOverrideGameOverMusic(SoundControl* sounds);
