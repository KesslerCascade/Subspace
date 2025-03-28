#include <stdbool.h>
#include <stdint.h>

typedef uintptr_t addr_t;

// client settings that are semt by the main process
typedef struct SubspaceClientSettings {
    char* gameDir;       // root directory where the game is located
    char* gameProgram;   // name of the game executable
    char* gamePath;      // fill path to game executable
} SubspaceClientSettings;

typedef struct TransientGlobalData {
    bool overrideHullText;
    int hull;
} TransientGlobalData;

extern SubspaceClientSettings scsettings;
TransientGlobalData tgd;

int sscmain(void);
