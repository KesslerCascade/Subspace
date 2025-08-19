#pragma once
#include "feature.h"
#include "subspacegame.h"

typedef struct SoundControl SoundControl;

typedef struct TweaksSettings {
    int targetfps;
    bool preserveload;
    bool savecompat;
    bool postgamesave;
    bool creditsmusic;
} TweaksSettings;

int tweaksGetTargetFPS();
void tweaksSetFrameTime();
bool tweaksPreserveLoad();
void tweaksPrepareSave();
void tweaksFinalizeSave();
void tweaksPostGameSave();
bool tweaksOverrideGameOverMusic(SoundControl* sounds);