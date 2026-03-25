#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

extern DisasmTrace DroneSystem_AddDrone_trace;

typedef Drone* (*FUNCTYPE(DroneSystem_AddDrone))(DroneSystem* self, Drone* drone, int slot);
DECLFUNC(DroneSystem_AddDrone);

typedef void (*FUNCTYPE(DroneSystem_ModifyDroneCount))(DroneSystem* self, int amount);
DECLFUNC(DroneSystem_ModifyDroneCount);

typedef bool (*FUNCTYPE(DroneSystem_PowerDrone))(DroneSystem* self, Drone* drone, bool userDriven,
                                                 bool force);
DECLFUNC(DroneSystem_PowerDrone);

typedef bool (*FUNCTYPE(DroneSystem_DePowerDrone))(DroneSystem* self, Drone* drone,
                                                   bool userDriven);
DECLFUNC(DroneSystem_DePowerDrone);

DECLSYM(DroneSystem_drones_offset);
// vector<Drone*>
#define DroneSystem_drones(ws) (&MEMBER(ftlbase, DroneSystem, ws, vector, drones))