#pragma once
#include "feature.h"
#include "subspacegame.h"

void practiceSetSave(const char *fn);
void practiceLoad();        // should only be called from CApp::OnLoop
void practiceLoad2();       // part 2
void practiceRenderNotice(void);