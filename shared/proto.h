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
} GameState;