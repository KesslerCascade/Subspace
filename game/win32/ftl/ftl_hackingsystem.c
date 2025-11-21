#include "ftl/drone.h"
#include "ftl/dronesystem.h"
#include "ftl/shipmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(HackingSystem_GetSpendDrone) = {
    SYMNAME("HackingSystem::GetSpendDrone"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN13HackingSystem13GetSpendDroneEv" },
             { 0 } }
};

FuncInfo FUNCINFO(HackingSystem_GetSpendDrone) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_INT
};