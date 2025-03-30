#pragma once
#include "subspacegame.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"

extern SubspaceFeature FrameAdv_feature;

void frameAdvStep(CommandGui *gui);
void frameAdvEndFrame(CApp *app); // to be called at the end of each frame