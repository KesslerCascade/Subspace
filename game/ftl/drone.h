#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

DECLSYM(Drone_blueprint_offset);
#define Drone_blueprint(drone) \
    (MEMBER(ftlbase, Drone, drone, DroneBlueprint*, blueprint))