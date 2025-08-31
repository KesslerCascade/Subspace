#pragma once

typedef enum LaunchFailReason {
    LAUNCH_FAIL_NOEXE = 1,
    LAUNCH_FAIL_REQPATCH,
    LAUNCH_FAIL_OTHER
} LaunchFailReason;
typedef enum LaunchMode { LAUNCH_PLAY = 0, LAUNCH_VALIDATE = 1 } LaunchMode;
typedef enum GameState {
    GAME_INIT,
    GAME_LOADING,
    GAME_MENU,
    GAME_RUN,
    GAME_PRACTICE,
    GAME_TUTORIAL
} GameState;
typedef enum ScreenshotEvent {
    SSEvent_Ach       = 0x0001,
    SSEvent_RFS1      = 0x0002,
    SSEvent_RFS2      = 0x0004,
    SSEvent_RFS3      = 0x0008,
    SSEvent_WinFight  = 0x0010,
    SSEvent_Destroyed = 0x0020,
    SSEvent_GameOver  = 0x0040,
    SSEvent_Victory   = 0x0080,
    SSEvent_Credits   = 0x0100
} ScreenshotEvent;
