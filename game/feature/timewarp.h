#pragma once
#include "ftl/commandgui.h"
#include "subspacegame.h"

extern SubspaceFeature TimeWarp_feature;

bool timeWarpBegin();
bool timeWarpIncrease();
bool timeWarpDecrease();
void timeWarpEnd();

void timeWarpRender();

float timeWarpAdjustSpeedFactor(float orig);
void timeWarpBeginFrame(CommandGui* gui);
