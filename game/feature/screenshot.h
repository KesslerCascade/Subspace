#pragma once
#include "feature.h"
#include "proto.h"
#include "subspacegame.h"

typedef struct ScreenshotSettings {
    bool sound;
    bool hidemouse;
    bool hidepause;
    bool hideinfoblock;
    int32_t events;
} ScreenshotSettings;

typedef struct CApp CApp;
void renderScreenshot(CApp* app, bool automatic);
void saveScreenshotFramebuf(int* fb);
void saveScreenshotFallback(void);
void screenshotCheckSound(void);

bool screenshotHideMouse(void);     // should mouse cursor be hidden in screenshot
bool screenshotHidePause(void);     // should pause text be hidden in screenshot
bool screenshotHideInfoBlock(void);   // should pause text be hidden in screenshot
bool screenshotUseFramebuf(void);   // if screenshot feature is using the framebuffer
bool screenshotAuto(int event);     // if we should take a screenshot when event happens
void screenshotCheckDestroyed(void);