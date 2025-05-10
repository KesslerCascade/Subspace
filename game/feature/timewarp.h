#pragma once
#include "ftl/commandgui.h"
#include "feature.h"
#include "subspacegame.h"

typedef struct TimeWarpSettings {
    int maxwarp;
    bool allowslowmo;
    int doubletap;
} TimeWarpSettings;

bool timeWarpBegin();
bool timeWarpIncrease();
bool timeWarpDecrease();
void timeWarpEnd();

void timeWarpRender();

float timeWarpAdjustSpeedFactor(float orig);
void timeWarpBeginFrame(CommandGui* gui);
