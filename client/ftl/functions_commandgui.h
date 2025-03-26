#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CommandGui CommandGui;

typedef void (*FUNCTYPE(CommandGui_KeyDown))(CommandGui* self, int key, bool shiftHeld);
DECLFUNC(CommandGui_KeyDown);

typedef void (*FUNCTYPE(CommandGui_SpaceBar))(CommandGui *self);
DECLFUNC(CommandGui_SpaceBar);

typedef bool (*FUNCTYPE(CommandGui_IsPaused))(CommandGui* self);
DECLFUNC(CommandGui_IsPaused);