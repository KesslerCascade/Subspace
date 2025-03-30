#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t addr_t;

// client settings that are semt by the main process
typedef struct SubspaceGameSettings {
    char* gameDir;       // root directory where the game is located
    char* gameProgram;   // name of the game executable
    char* gamePath;      // fill path to game executable
} SubspaceGameSettings;

typedef struct GameState {
    // Numeric Hull
    bool overrideHullText;
    int hull;

    // Time Warp
    bool timeWarpActive;
    float warpFactor;
    float warpFactorActual;        // set by CFPS if we're capped due to framerate
    bool warpOriginalVSync;        // whether vsync was on or off when time warp began
    bool warpOriginalFrameLimit;   // whether frame limit was on or off when time warp began
} GameState;

extern SubspaceGameSettings settings;
GameState gs;

int sscmain(void);
