#pragma once
#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "feature.h"
#include "subspacegame.h"

void frameAdvStep(CommandGui *gui);
void frameAdvEndFrame(CommandGui* gui);   // to be called at the end of each frame
