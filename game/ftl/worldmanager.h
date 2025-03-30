#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct WorldManager WorldManager;

int subspace_WorldManager_ctor_pre(WorldManager* self);

// FTL functions & hooks below

typedef int (*FUNCTYPE(WorldManager_ctor))(WorldManager* self);
DECLFUNC(WorldManager_ctor);

typedef int (*FUNCTYPE(WorldManager_OnInit))(WorldManager* self);
DECLFUNC(WorldManager_OnInit);
