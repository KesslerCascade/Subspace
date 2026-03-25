#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "hook/disasmtrace.h"

Symbol SYM(Drone_blueprint_offset) = {
    SYMNAME("Drone->blueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &DroneSystem_AddDrone_trace }, { 0 } }
};