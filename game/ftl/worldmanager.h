#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct WorldManager WorldManager;
typedef struct ShipManager ShipManager;
typedef struct CompleteShip CompleteShip;
typedef struct ShipEvent ShipEvent;

int subspace_WorldManager_ctor_pre(WorldManager* self);

// FTL functions & hooks below

extern DisasmTrace WorldManager_CreateShip_trace;
extern DisasmTrace WorldManager_StartGame_trace;

typedef int (*FUNCTYPE(WorldManager_ctor))(WorldManager* self);
DECLFUNC(WorldManager_ctor);

typedef int (*FUNCTYPE(WorldManager_OnInit))(WorldManager* self);
DECLFUNC(WorldManager_OnInit);

typedef void (*FUNCTYPE(WorldManager_StartGame))(WorldManager* self, ShipManager* newShip);
DECLFUNC(WorldManager_StartGame);

typedef CompleteShip* (*FUNCTYPE(WorldManager_CreateShip))(WorldManager* self, ShipEvent* ship,
                                                           bool boss);
DECLFUNC(WorldManager_CreateShip);

DECLSYM(WorldManager_bossShip_offset);
