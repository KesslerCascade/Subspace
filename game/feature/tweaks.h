#pragma once
#include "feature.h"
#include "subspacegame.h"

typedef struct TweaksSettings {
    int targetfps;
} TweaksSettings;

int tweaksGetTargetFPS();
void tweaksSetFrameTime();