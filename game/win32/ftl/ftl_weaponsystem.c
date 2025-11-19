#include "ftl/projectilefactory.h"
#include "ftl/shipmanager.h"
#include "ftl/shipsystem.h"
#include "ftl/weaponsystem.h"
#include "hook/disasmtrace.h"

Symbol SYM(WeaponSystem_PowerWeapon) = {
    SYMNAME("WeaponSystem::PowerWeapon"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_PowerWeapon_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name = "_ZN12WeaponSystem11PowerWeaponEP17ProjectileFactorybb" },
             { 0 } }
};
FuncInfo FUNCINFO(WeaponSystem_PowerWeapon) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_INT, 0, true },
                { 4, ARG_INT, 0, true } }
};

DisasmTrace WeaponSystem_PowerWeapon_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(WeaponSystem_PowerWeapon),
    .ops  = { { DT_OP(SKIP), .imin = 14, .imax = 22 },
             { I_CALL },   // CALL ShipSystem::GetLocked
              { DT_OP(SKIP), .imin = 1, .imax = 9 },
             { I_CMP },
             { DT_OP(SKIP), .imin = 0, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { 0, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 4 } },
                .argout = { 0, DT_OUT_SYM1 },
                .argcap = { DT_CAPTURE1 } },   // weapon->blueprint
              { I_MOV, .argf = { 0, ARG_REG }, .argcap = { DT_CAPTURE2, DT_MATCH1 } },
             { I_TEST, .argf = { ARG_MATCH, ARG_MATCH }, .argcap = { DT_MATCH2, DT_MATCH2 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ProjectileFactory_blueprint_offset) }
};

Symbol SYM(WeaponSystem_weapons_offset) = {
    SYMNAME("WeaponSystem->weapons"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_GetWeaponTotal_trace }, { 0 } }
};

DisasmTrace WeaponSystem_OnLoop_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ProjectileFactory_Update),
    .mod  = DTRACE_MOD_FUNCSTART,
    // disambiguate from ArtillerySystem::OnLoop, the other function that calls
    // ProjectileFactory::Update
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 6, .imax = 12 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },           // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL ShipSystem::OnLoop
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { 0, ARG_ADDR },
                .argsym = { 0, &SYM(WeaponSystem_weapons_offset) } },   // this->weapons
              { DT_OP(FINISH) } },
    .out  = { &SYM(WeaponSystem_OnLoop),                                // DT_OUT_SYM1
              &SYM(ShipSystem_OnLoop) }
};

Symbol SYM(WeaponSystem_OnLoop) = {
    SYMNAME("WeaponSystem::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WeaponSystem_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WeaponSystem6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(WeaponSystem_OnLoop) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_VOID };
