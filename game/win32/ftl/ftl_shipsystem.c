#include "ftl/shipmanager.h"
#include "ftl/shipsystem.h"
#include "ftl/weaponsystem.h"
#include "hook/disasmtrace.h"

Symbol SYM(ShipSystem_GetExploded) = {
    SYMNAME("ShipSystem::GetExploded"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipSystem11GetExplodedEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipSystem_GetExploded) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_INT
};

Symbol SYM(ShipSystem_OnLoop) = {
    SYMNAME("ShipSystem::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WeaponSystem_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10ShipSystem6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipSystem_OnLoop) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                         .rettype = RET_VOID };
