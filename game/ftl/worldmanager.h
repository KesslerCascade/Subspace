#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct WorldManager WorldManager;
typedef struct ShipManager ShipManager;
typedef struct CompleteShip CompleteShip;
typedef struct ShipEvent ShipEvent;
typedef struct StarMap StarMap;

int subspace_WorldManager_ctor_pre(WorldManager* self);

// FTL functions & hooks below

extern DisasmTrace WorldManager_CreateShip_trace;
extern DisasmTrace WorldManager_StartGame_trace;
extern DisasmTrace WorldManager_OnInit_trace;

typedef int (*FUNCTYPE(WorldManager_ctor))(WorldManager* self);
DECLFUNC(WorldManager_ctor);

typedef int (*FUNCTYPE(WorldManager_OnInit))(WorldManager* self);
DECLFUNC(WorldManager_OnInit);

typedef void (*FUNCTYPE(WorldManager_StartGame))(WorldManager* self, ShipManager* newShip);
DECLFUNC(WorldManager_StartGame);

typedef void (*FUNCTYPE(WorldManager_LoadGame))(WorldManager* self, basic_string* file);
DECLFUNC(WorldManager_LoadGame);

typedef CompleteShip* (*FUNCTYPE(WorldManager_CreateShip))(WorldManager* self, ShipEvent* ship,
                                                           bool boss);
DECLFUNC(WorldManager_CreateShip);

DECLSYM(WorldManager_bossShip_offset);

DECLSYM(WorldManager_starMap_offset);
#define WorldManager_starMap(worldmgr) (&MEMBER(ftlbase, WorldManager, worldmgr, StarMap, starMap))

DECLSYM(WorldManager_starMap_worldLevel_offset);
#define WorldManager_worldLevel(worldmgr) \
    (*(&MEMBER(ftlbase, WorldManager, worldmgr, int, starMap_worldLevel)))
