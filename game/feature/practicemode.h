#pragma once
#include "feature.h"
#include "subspacegame.h"

void practiceSetSave(strref fn);
void practiceLoad();   // should only be called from CApp::OnLoop
void practiceRenderNotice(void);