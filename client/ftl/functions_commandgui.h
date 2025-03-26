#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CommandGui CommandGui;

typedef void (*FUNCTYPE(CommandGui_KeyDown))(CommandGui* self, int key, bool shiftHeld);
DECLFUNC(CommandGui_KeyDown);

typedef void (*FUNCTYPE(CommandGui_SetPaused))(CommandGui* self, bool paused, bool autoPaused);
DECLFUNC(CommandGui_SetPaused);
#define CommandGui_SetPaused(self, paused, autoPaused) \
    FCALL(ftlbase, CommandGui_SetPaused, self, paused, autoPaused)

typedef bool (*FUNCTYPE(CommandGui_IsPaused))(CommandGui* self);
DECLFUNC(CommandGui_IsPaused);