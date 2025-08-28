#pragma once
#include "feature.h"
#include "subspacegame.h"

typedef struct ScreenshotSettings {
    bool hidemouse;
    bool hidepause;
    bool hideinfoblock;
} ScreenshotSettings;

typedef struct CApp CApp;
void renderScreenshot(CApp* app, bool automatic);
void saveScreenshotFramebuf(int* fb);
void saveScreenshotFallback(void);

bool screenshotHideMouse(void);     // should mouse cursor be hidden in screenshot
bool screenshotHidePause(void);     // should pause text be hidden in screenshot
bool screenshotHideInfoBlock(void);   // should pause text be hidden in screenshot
bool screenshotUseFramebuf(void);   // if screenshot feature is using the framebuffer
