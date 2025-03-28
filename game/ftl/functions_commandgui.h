#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct CommandGui CommandGui;

extern DisasmTrace CommandGui_SpaceBar_trace;
extern DisasmTrace CommandGui_RenderStatic_trace;
extern DisasmTrace CommandGui_RunCommand_HULL_trace;
extern DisasmTrace CommandGui_RunCommand_DELETE_trace;
extern DisasmTrace CommandGui_Restart_trace;

typedef void (*FUNCTYPE(CommandGui_KeyDown))(CommandGui* self, int key, bool shiftHeld);
DECLFUNC(CommandGui_KeyDown);

typedef void (*FUNCTYPE(CommandGui_SetPaused))(CommandGui* self, bool paused, bool autoPaused);
DECLFUNC(CommandGui_SetPaused);
#define CommandGui_SetPaused(self, paused, autoPaused) \
    FCALL(ftlbase, CommandGui_SetPaused, self, paused, autoPaused)

typedef bool (*FUNCTYPE(CommandGui_IsPaused))(CommandGui* self);
DECLFUNC(CommandGui_IsPaused);

typedef bool (*FUNCTYPE(CommandGui_IsGameOver))(CommandGui* self);
DECLFUNC(CommandGui_IsGameOver);

typedef void (*FUNCTYPE(CommandGui_RenderStatic))(CommandGui* self);
DECLFUNC(CommandGui_RenderStatic);

typedef void (*FUNCTYPE(CommandGui_Restart))(CommandGui* self);
DECLFUNC(CommandGui_Restart);

DECLSYM(CommandGui_SpaceBar);

DECLSYM(CommandGui_shipComplete_offset);

#define CommandGui_shipComplete(cgui) MEMBER(ftlbase, CommandGui, cgui, CompleteShip*, shipComplete)
// convenience macro since we almost always want the ShipManager
#define CommandGui_ship(cgui)                                                   \
    (MEMBER(ftlbase, CommandGui, cgui, CompleteShip*, shipComplete) ?           \
         MEMBER(ftlbase,                                                        \
                CompleteShip,                                                   \
                MEMBER(ftlbase, CommandGui, cgui, CompleteShip*, shipComplete), \
                ShipManager*,                                                   \
                shipManager) :                                                  \
         0)
