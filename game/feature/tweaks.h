#pragma once
#include "feature.h"
#include "subspacegame.h"

typedef struct TweaksSettings {
    int targetfps;
    bool preserveload;
    bool savecompat;
} TweaksSettings;

int tweaksGetTargetFPS();
void tweaksSetFrameTime();
bool tweaksPreserveLoad();
void tweaksPrepareSave();
void tweaksFinalizeSave();