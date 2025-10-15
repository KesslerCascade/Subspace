#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct CommandGui CommandGui;
typedef struct StarMap StarMap;
typedef struct Equipment Equipment;

int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld);
int subspace_CommandGui_OnLoop_pre(CommandGui* self);
void subspace_CommandGui_OnLoop_post(CommandGui* self);
void subspace_CommandGui_RenderStatic_post(CommandGui* self);

// FTL functions & wrappers below

extern DisasmTrace CommandGui_SpaceBar_trace;
extern DisasmTrace CommandGui_RenderStatic_trace;
extern DisasmTrace CommandGui_RunCommand_HULL_trace;
extern DisasmTrace CommandGui_RunCommand_DELETE_trace;
extern DisasmTrace CommandGui_RunCommand_SHIP_trace;
extern DisasmTrace CommandGui_RunCommand_WEAPON_trace;
extern DisasmTrace CommandGui_Restart_trace;
extern DisasmTrace CommandGui_CheckGameOver_trace;
extern DisasmTrace CommandGui_CheckGameOver_trace_2;
extern DisasmTrace CommandGui_OnLoop_trace;
extern DisasmTrace CommandGui_OnLoop_GameOverLoop_trace;
extern DisasmTrace CommandGui_ForceJumpComplete_trace;

typedef void (*FUNCTYPE(CommandGui_KeyDown))(CommandGui* self, int key, bool shiftHeld);
DECLFUNC(CommandGui_KeyDown);

typedef void (*FUNCTYPE(CommandGui_SetPaused))(CommandGui* self, bool paused, bool autoPaused);
DECLFUNC(CommandGui_SetPaused);
#define CommandGui_SetPaused(self, paused, autoPaused) \
    FCALL(ftlbase, CommandGui_SetPaused, self, paused, autoPaused)

typedef bool (*FUNCTYPE(CommandGui_IsPaused))(CommandGui* self);
DECLFUNC(CommandGui_IsPaused);
#define CommandGui_IsPaused(self) FCALL(ftlbase, CommandGui_IsPaused, self)

typedef void (*FUNCTYPE(CommandGui_RenderPause))(CommandGui* self);
DECLFUNC(CommandGui_RenderPause);

typedef bool (*FUNCTYPE(CommandGui_IsGameOver))(CommandGui* self);
DECLFUNC(CommandGui_IsGameOver);
#define CommandGui_IsGameOver(self) FCALL(ftlbase, CommandGui_IsGameOver, self)

typedef void (*FUNCTYPE(CommandGui_CheckGameOver))(CommandGui* self);
DECLFUNC(CommandGui_CheckGameOver);

typedef bool (*FUNCTYPE(CommandGui_IsJumpComplete))(CommandGui* self);
DECLFUNC(CommandGui_IsJumpComplete);
#define CommandGui_IsJumpComplete(self) FCALL(ftlbase, CommandGui_IsJumpComplete, self)

typedef void (*FUNCTYPE(CommandGui_ForceJumpComplete))(CommandGui* self);
DECLFUNC(CommandGui_ForceJumpComplete);
#define CommandGui_ForceJumpComplete(self) FCALL(ftlbase, CommandGui_ForceJumpComplete, self)

typedef bool (*FUNCTYPE(CommandGui_CanSave))(CommandGui* self);
DECLFUNC(CommandGui_CanSave);
#define CommandGui_CanSave(self) FCALL(ftlbase, CommandGui_CanSave, self)

typedef void (*FUNCTYPE(CommandGui_RenderStatic))(CommandGui* self);
DECLFUNC(CommandGui_RenderStatic);

typedef void (*FUNCTYPE(CommandGui_Restart))(CommandGui* self);
DECLFUNC(CommandGui_Restart);
#define CommandGui_Restart(self) FCALL(ftlbase, CommandGui_Restart, self)

typedef void (*FUNCTYPE(CommandGui_OnLoop))(CommandGui* self);
DECLFUNC(CommandGui_OnLoop);

typedef void (*FUNCTYPE(CommandGui_OnInit))(CommandGui* self);
DECLFUNC(CommandGui_OnInit);

typedef void (*FUNCTYPE(CommandGui_LinkMap))(CommandGui* self, StarMap* map);
DECLFUNC(CommandGui_LinkMap);

typedef int (*FUNCTYPE(CommandGui_GetCommand))(CommandGui* self);
DECLFUNC(CommandGui_GetCommand);

typedef int (*FUNCTYPE(CommandGui_OnCleanup))(CommandGui* self);
DECLFUNC(CommandGui_OnCleanup);
#define CommandGui_OnCleanup(self) FCALL(ftlbase, CommandGui_OnCleanup, self)

DECLSYM(CommandGui_SpaceBar);
DECLSYM(CommandGui_shipComplete_offset);
DECLSYM(CommandGui_gameOverScreen_offset);
DECLSYM(CommandGui_starMap_offset);
DECLSYM(CommandGui_newLocation_offset);
DECLSYM(CommandGui_equipScreen_offset);

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

#define CommandGui_equipScreen(cgui) MEMBER(ftlbase, CommandGui, cgui, Equipment*, equipScreen)
