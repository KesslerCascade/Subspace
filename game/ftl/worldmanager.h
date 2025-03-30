#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

typedef struct WorldManager WorldManager;

typedef int (*FUNCTYPE(WorldManager_ctor))(WorldManager* self);
DECLFUNC(WorldManager_ctor);

typedef int (*FUNCTYPE(WorldManager_OnInit))(WorldManager* self);
DECLFUNC(WorldManager_OnInit);
