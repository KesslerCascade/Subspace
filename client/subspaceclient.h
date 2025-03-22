#include <stdbool.h>
#include <stdint.h>

// client settings that are semt by the main process
typedef struct SubspaceClientSettings {
    char* gameDir;       // root directory where the game is located
    char* gameProgram;   // name of the game executable
    char* gamePath;      // fill path to game executable
} SubspaceClientSettings;

extern SubspaceClientSettings scsettings;
