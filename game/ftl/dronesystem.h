#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace DroneSystem_AddDrone_trace;

typedef Drone* (*FUNCTYPE(DroneSystem_AddDrone))(DroneSystem* self, Drone *drone, int slot);
DECLFUNC(DroneSystem_AddDrone);

DECLSYM(DroneSystem_drones_offset);
// vector<Drone*>
#define DroneSystem_drones(ws) (&MEMBER(ftlbase, DroneSystem, ws, vector, drones))