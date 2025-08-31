#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct WorldManager WorldManager;
typedef struct ShipManager ShipManager;
typedef struct CompleteShip CompleteShip;
typedef struct ShipEvent ShipEvent;
typedef struct StarMap StarMap;
typedef struct Location Location;
typedef struct LocationEvent LocationEvent;
typedef struct CommandGui CommandGui;

int subspace_WorldManager_ctor_pre(WorldManager* self);

// FTL functions & hooks below

extern DisasmTrace WorldManager_CreateShip_trace;
extern DisasmTrace WorldManager_StartGame_trace;
extern DisasmTrace WorldManager_OnInit_trace;
extern DisasmTrace WorldManager_Restart_trace;
extern DisasmTrace WorldManager_CreateNewGame_trace;
extern DisasmTrace WorldManager_CheckForNewLocation_trace;
extern DisasmTrace WorldManager_CheckForNewLocation_trace_2;
extern DisasmTrace WorldManager_PrepareAutoSave_trace;
extern DisasmTrace WorldManager_PauseLoop_trace;
extern DisasmTrace WorldManager_ModifyResources_trace;
extern DisasmTrace WorldManager_OnLoop_trace;
extern DisasmTrace WorldManager_OnLoop_Mantis_trace;
extern DisasmTrace WorldManager_CreateLocation_trace;

typedef int (*FUNCTYPE(WorldManager_ctor))(WorldManager* self);
DECLFUNC(WorldManager_ctor);

typedef int (*FUNCTYPE(WorldManager_OnInit))(WorldManager* self);
DECLFUNC(WorldManager_OnInit);

typedef void (*FUNCTYPE(WorldManager_OnLoop))(WorldManager* self);
DECLFUNC(WorldManager_OnLoop);

typedef void (*FUNCTYPE(WorldManager_StartGame))(WorldManager* self, ShipManager* newShip);
DECLFUNC(WorldManager_StartGame);
#define WorldManager_StartGame(self, newShip) FCALL(ftlbase, WorldManager_StartGame, self, newShip)

typedef void (*FUNCTYPE(WorldManager_LoadGame))(WorldManager* self, basic_string* file);
DECLFUNC(WorldManager_LoadGame);
#define WorldManager_LoadGame(self, file) FCALL(ftlbase, WorldManager_LoadGame, self, file)

typedef void (*FUNCTYPE(WorldManager_SaveGame))(WorldManager* self);
DECLFUNC(WorldManager_SaveGame);
#define WorldManager_SaveGame(self) FCALL(ftlbase, WorldManager_SaveGame, self)

typedef void (*FUNCTYPE(WorldManager_PrepareAutoSave))(WorldManager* self);
DECLFUNC(WorldManager_PrepareAutoSave);

typedef void (*FUNCTYPE(WorldManager_Restart))(WorldManager* self);
DECLFUNC(WorldManager_Restart);

typedef void (*FUNCTYPE(WorldManager_CreateNewGame))(WorldManager* self);
DECLFUNC(WorldManager_CreateNewGame);

typedef void (*FUNCTYPE(WorldManager_ClearLocation))(WorldManager* self);
DECLFUNC(WorldManager_ClearLocation);
#define WorldManager_ClearLocation(self) FCALL(ftlbase, WorldManager_ClearLocation, self)

typedef void (*FUNCTYPE(WorldManager_CreateLocation))(WorldManager* self, Location* loc);
DECLFUNC(WorldManager_CreateLocation);

typedef void (*FUNCTYPE(WorldManager_UpdateLocation))(WorldManager* self, LocationEvent* loc);
DECLFUNC(WorldManager_UpdateLocation);

typedef CompleteShip* (*FUNCTYPE(WorldManager_CreateShip))(WorldManager* self, ShipEvent* ship,
                                                           bool boss);
DECLFUNC(WorldManager_CreateShip);

DECLSYM(WorldManager_playerShip_offset);
#define WorldManager_playerShip(worldmgr) \
    *(&MEMBER(ftlbase, WorldManager, worldmgr, CompleteShip*, playerShip))

DECLSYM(WorldManager_bossShip_offset);
#define WorldManager_bossShip(worldmgr) \
    *(&MEMBER(ftlbase, WorldManager, worldmgr, BossShip*, bossShip))

DECLSYM(WorldManager_starMap_offset);
#define WorldManager_starMap(worldmgr) (&MEMBER(ftlbase, WorldManager, worldmgr, StarMap, starMap))

DECLSYM(WorldManager_starMap_worldLevel_offset);
#define WorldManager_worldLevel(worldmgr) \
    (*(&MEMBER(ftlbase, WorldManager, worldmgr, int, starMap_worldLevel)))

DECLSYM(WorldManager_commandGui_offset);
#define WorldManager_commandGui(worldmgr) \
    (*(&MEMBER(ftlbase, WorldManager, worldmgr, CommandGui*, commandGui)))

typedef void (*FUNCTYPE(WorldManager_PauseLoop))(WorldManager* self);
DECLFUNC(WorldManager_PauseLoop);

typedef bool (*FUNCTYPE(WorldManager_CheckForNewLocation))(WorldManager* self, bool savingGame);
DECLFUNC(WorldManager_CheckForNewLocation);

typedef LocationEvent* (*FUNCTYPE(WorldManager_ModifyResources))(WorldManager* self,
                                                                 LocationEvent* event);
DECLFUNC(WorldManager_ModifyResources);