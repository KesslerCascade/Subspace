#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "ftl/shipmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(DroneSystem_AddDrone);
Symbol SYM(DroneSystem_AddDrone) = {
    SYMNAME("DroneSystem::AddDrone"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_AddDrone_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11DroneSystem8AddDroneEP5Dronei" },
             { 0 } }
};

FuncInfo FUNCINFO(DroneSystem_AddDrone) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype = RET_PTR
};

DisasmTrace DroneSystem_AddDrone_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(DroneSystem_AddDrone),
    .ops  = { { DT_OP(SKIP), .imin = 14, .imax = 24 },
             { I_MOV,
                .argf   = { 0, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 4 } },
                .argcap = { DT_CAPTURE1 } },   // drone pointer
              { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { DT_CAPTURE2, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM1 } },   // drone->blueprint
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ESP } },
                .argcap = { 0, DT_MATCH2 } },
             { I_CALL },   // CALL DroneBlueprint::GetEquipmentName
              { DT_OP(FINISH) } },
    .out  = { &SYM(Drone_blueprint_offset) }
};

Symbol SYM(DroneSystem_drones_offset) = {
    SYMNAME("DroneSystem->drones"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_GetDroneTotal_trace }, { 0 } }
};

Symbol SYM(DroneSystem_ModifyDroneCount) = {
    SYMNAME("DroneSystem::ModifyDroneCount"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_ModifyDroneCount_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11DroneSystem16ModifyDroneCountEi" },
             { 0 } }
};

FuncInfo FUNCINFO(DroneSystem_ModifyDroneCount) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};