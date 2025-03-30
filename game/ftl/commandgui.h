#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CommandGui CommandGui;

int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld);
int subspace_CommandGui_OnLoop_pre(CommandGui* self);

// FTL functions & wrappers below

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
#define CommandGui_IsPaused(self) FCALL(ftlbase, CommandGui_IsPaused, self)

typedef bool (*FUNCTYPE(CommandGui_IsGameOver))(CommandGui* self);
DECLFUNC(CommandGui_IsGameOver);
#define CommandGui_IsGameOver(self) FCALL(ftlbase, CommandGui_IsGameOver, self)

typedef void (*FUNCTYPE(CommandGui_RenderStatic))(CommandGui* self);
DECLFUNC(CommandGui_RenderStatic);

typedef void (*FUNCTYPE(CommandGui_Restart))(CommandGui* self);
DECLFUNC(CommandGui_Restart);

typedef void (*FUNCTYPE(CommandGui_OnLoop))(CommandGui* self);
DECLFUNC(CommandGui_OnLoop);

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
