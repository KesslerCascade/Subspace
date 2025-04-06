#pragma once
#include "patch.h"

extern Patch patch_CApp_OnExecute;
extern Patch patch_CApp_OnLoop;
extern Patch patch_CApp_OnKeyDown;
extern Patch patch_CFPS_OnLoop;
extern Patch patch_CFPS_TargetFrameTime;
extern Patch patch_CombatControl_RenderTarget;
extern Patch patch_CommandGui_KeyDown;
extern Patch patch_CommandGui_OnLoop;
extern Patch patch_CommandGui_RenderStatic;
extern Patch patch_FTLButton_OnRender;
extern Patch patch_MouseControl_OnRender;
extern Patch patch_ShipStatus_RenderHealth;
extern Patch patch_ShipStatus_OnRender;
extern Patch patch_TextLibrary_GetText;