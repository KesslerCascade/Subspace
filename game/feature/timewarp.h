#pragma once
#include "ftl/commandgui.h"
#include "feature.h"
#include "subspacegame.h"

bool timeWarpBegin();
bool timeWarpIncrease();
bool timeWarpDecrease();
void timeWarpEnd();

void timeWarpRender();

float timeWarpAdjustSpeedFactor(float orig);
void timeWarpBeginFrame(CommandGui* gui);
