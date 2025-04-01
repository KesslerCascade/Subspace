#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t addr_t;

typedef struct SubspaceFeature SubspaceFeature;
typedef struct PatchState PatchState;
typedef struct Symbol Symbol;
// Return how much space is needed for any feature-specific settings
typedef size_t (*featureSettingsSize_t)(SubspaceFeature* feat);

// Try to apply the necessary patches for the feature and reutrn if it's available
typedef bool (*featurePatch_t)(SubspaceFeature* feat, void* settings, PatchState* ps);

// Enable or disable the feature at runtime
typedef bool (*featureEnable_t)(SubspaceFeature* feat, void* settings, bool enabled);

#ifdef _DEBUG
extern void WriteDbg(const char* str);
#else
#define WriteDbg(x)
#endif

extern const int subspace_version_maj;
extern const int subspace_version_min;
extern const char* subspace_version_series;
extern const char* subspace_version_str;

typedef struct SubspaceFeature {
    bool available;   // is this feature available -- i.e. did all the necessary patches apply?
    bool enabled;   // is the feature enabled? Doesn't mean that it's active right now, just enabled

    const char* name;   // name of the feature
    featureSettingsSize_t settingsSize;
    featurePatch_t patch;
    featureEnable_t enable;

    void* settings;              // memory allocated for a feature-specific struct

    Symbol* requiredSymbols[];   // other required symbols that aren't checked for during patch
                                 // sequence validation
} SubspaceFeature;

// client settings that are semt by the main process
typedef struct SubspaceGameSettings {
    char* gameDir;       // root directory where the game is located
    char* gameProgram;   // name of the game executable
    char* gamePath;      // fill path to game executable

    bool testMode;

    SubspaceFeature* numericHull;
    SubspaceFeature* frameAdv;
    SubspaceFeature* timeWarp;
} SubspaceGameSettings;

typedef struct GameState {
    // Numeric Hull
    bool overrideHullText;
    int hull;

    // Frame Advance step in progress
    bool frameAdvStep;

    // Time Warp
    bool timeWarpActive;
    float warpFactor;
    float warpFactorActual;   // set by CFPS if we're capped due to framerate
    double avgFrameMS;        // milliseconds per frame, averaged over the last several frames
} GameState;

extern SubspaceGameSettings settings;
extern GameState gs;

int sscmain(void);
bool initFeature(SubspaceFeature* feat, PatchState* ps);
bool enableFeature(SubspaceFeature* feat, bool enabled);
