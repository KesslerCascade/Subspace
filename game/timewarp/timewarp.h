#pragma once
#include "subspacegame.h"

void timeWarpBegin();
bool timeWarpIncrease();
bool timeWarpDecrease();
void timeWarpEnd();
void timeWarpFrameCalcActual(float speedFactor);