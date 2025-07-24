#include "ftl/commandgui.h"
#include "ftl/ship.h"
#include "ftl/shipmanager.h"
#include "ftl/shipstatus.h"
#include "ftl/shipsystem.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

INITWRAP(ShipManager_DamageHull);
Symbol SYM(ShipManager_DamageHull) = {
    SYMNAME("ShipManager::DamageHull"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RunCommand_HULL_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ShipManager10DamageHullEib" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_DamageHull) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};

INITWRAP(ShipManager_GetIsJumping);
Symbol SYM(ShipManager_GetIsJumping) = {
    SYMNAME("ShipManager::GetIsJumping"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "EAX,_ZN11ShipManager12GetIsJumpingEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_GetIsJumping) = { .nargs   = 1,
                                                .stdcall = true,
                                                .args    = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace ShipManager_DamageHull_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipManager_DamageHull),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // capture reg this pointer is saved in
              { DT_OP(SKIP), .imin = 6, .imax = 12 },
             { I_MOVZX },
             { I_CMP },
             { DT_OP(JMP) },   // follow whichever conditional jump is used
              { I_LEA,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM1 } },   // offset of Ship within ShipManager
              { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_ship_offset), &SYM(Ship_DamageHull) }
};

Symbol SYM(ShipManager_ship_offset) = {
    SYMNAME("ShipManager->ship"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageHull_trace }, { 0 } }
};

Symbol SYM(ShipManager_current_target_offset) = {
    SYMNAME("ShipManager->current_target"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_SpaceBar_trace }, { 0 } }
};

Symbol SYM(ShipManager_bDestroyed_offset) = {
    SYMNAME("ShipManager->bDestroyed"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { 0 } }
};

Symbol SYM(ShipManager_OnInit) = {
    SYMNAME("ShipManager::OnInit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CompleteShip_OnInit_trace }, { 0 } }
};

DisasmTrace ShipManager_OnInit_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ShipManager_OnInit),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_myBlueprint_offset) }
};

Symbol SYM(ShipManager_currentScrap_offset) = {
    SYMNAME("ShipManager->currentScrap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v2 },
             { 0 } }
};

Symbol SYM(ShipManager_myBlueprint_offset) = {
    SYMNAME("ShipManager->myBlueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_OnInit_trace }, { 0 } }
};

// offset within the embedded ShipBlueprint structure
Symbol SYM(ShipManager_myBlueprint_name_offset) = {
    SYMNAME("ShipManager->myBlueprint->name"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_StartGame_trace }, { 0 } }
};

// offset within the embedded ShipBlueprint structure
Symbol SYM(ShipManager_myBlueprint_blueprintName_offset) = {
    SYMNAME("ShipManager->myBlueprint->blueprintName"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v2 },
             { 0 } }
};

INITWRAP(ShipManager_GetDroneCount);
Symbol SYM(ShipManager_GetDroneCount) = {
    SYMNAME("ShipManager::GetDroneCount"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "EAX,_ZN11ShipManager13GetDroneCountEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_GetDroneCount) = { .nargs   = 1,
                                                 .stdcall = true,
                                                 .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(ShipManager_GetMissileCount);
Symbol SYM(ShipManager_GetMissileCount) = {
    SYMNAME("ShipManager::GetMissileCount"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipStatus_LinkShip_trace_v2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "EAX,_ZN11ShipManager15GetMissileCountEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_GetMissileCount) = { .nargs   = 1,
                                                   .stdcall = true,
                                                   .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(ShipManager_HasEquipment);
Symbol SYM(ShipManager_HasEquipment) = {
    SYMNAME("ShipManager::HasEquipment"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CheckForNewLocation_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "EAX,_ZN10ShipObject12HasEquipmentENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_HasEquipment) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } }
};

INITWRAP(ShipManager_ModifyScrapCount);
Symbol SYM(ShipManager_ModifyScrapCount) = {
    SYMNAME("ShipManager::ModifyScrapCount"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_ModifyResources_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "EAX,_ZN11ShipManager16ModifyScrapCountEib" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_ModifyScrapCount) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

DisasmTrace ShipManager_DamageSystem_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ION_ARMOR",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 7, .imax = 15 },
             // just the minimum necessary to disambiguate this from ShipMnaager::PulsarDamage
              { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0 } } },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x4 } } },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x18 } } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_DamageSystem) },
};

Symbol SYM(ShipManager_DamageSystem) = {
    SYMNAME("ShipManager::DamageSystem"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageSystem_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ShipManager12DamageSystemEi6Damage" },
             { 0 } }
};

DisasmTrace ShipManager_SunDamage_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(ShipManager_DamageSystem),
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 149, .imax = 199 },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_EBP }, { .addr = 0xffffffff } } },   // damage.ownerId = -1
              { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_EBP }, { .addr = 0xffffffff } } },   // damage.selfId = -1
              { DT_OP(SKIP), .imin = 8, .imax = 18 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = &SYM(ShipManager_DamageSystem) },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf = { ARG_REG, ARG_ADDR },
                .args = { { REG_ESP }, { .addr = 0x3f800000 } } },   // 1.0 damage
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },                 // CALL Ship::ProjectileStrike
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_SunDamage),                           // DT_OUT_SYM1
              &SYM(Ship_ProjectileStrike) }
};

Symbol SYM(ShipManager_SunDamage) = {
    SYMNAME("ShipManager::SunDamage"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_SunDamage_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ShipManager9SunDamageEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_SunDamage) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

Symbol SYM(ShipManager_Wait) = {
    SYMNAME("ShipManager::Wait"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ShipManager4WaitEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_Wait) = { .nargs   = 1,
                                        .stdcall = true,
                                        .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                        .rettype = RET_VOID };

DisasmTrace ShipManager_JumpLeave_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "jumpLeave",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 7, .imax = 14 },
             { I_SUB,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX }, { .addr = 1 } },
                .argout = { DT_OUT_SYM2 } },   // this->fuel_count -= 1
              { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_JumpLeave),     // DT_OUT_SYM1
              &SYM(ShipManager_fuel_count_offset) }
};

Symbol SYM(ShipManager_JumpLeave) = {
    SYMNAME("ShipManager::JumpLeave"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_JumpLeave_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ShipManager9JumpLeaveEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_JumpLeave) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

Symbol SYM(ShipManager_fuel_count_offset) = {
    SYMNAME("ShipManager->fuel_count"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_JumpLeave_trace }, { 0 } }
};

DisasmTrace ShipManager_DamageArea_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ACH_CRYSTAL_SHARD",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = { DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 4, .imax = 14 },
             { I_ADD,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(ShipManager_ship_offset) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_DamageArea) }
};

// alternate form found in some versions
DisasmTrace ShipManager_DamageArea_trace_2 = {
    .c    = DTRACE_STRREFS,
    .cstr = "ACH_CRYSTAL_SHARD",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = { DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 28, .imax = 38 },
             { I_LEA, .argf = { 0, ARG_ADDR }, .argsym = { 0, &SYM(ShipManager_ship_offset) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_DamageArea) }
};

Symbol SYM(ShipManager_DamageArea) = {
    SYMNAME("ShipManager::DamageArea"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageArea_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageArea_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_DamageArea) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 8, ARG_STRUCT, 0, true },
                { 52, ARG_STRUCT, 0, true },
                { 4, ARG_INT, 0, true } },
    .rettype = RET_INT
};

DisasmTrace ShipManager_DamageBeam_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ACH_SLUG_BIO",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = { DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 4, .imax = 14 },
             { I_SUB, .argf = { ARG_REG }, .args = { { REG_ESP } } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_DamageBeam) }
};

Symbol SYM(ShipManager_DamageBeam) = {
    SYMNAME("ShipManager::DamageBeam"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_DamageBeam_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name = "_ZN11ShipManager10DamageAreaE6Pointf6Damageb" },
             { 0 } }
};
FuncInfo FUNCINFO(ShipManager_DamageBeam) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 8, ARG_STRUCT, 0, true },
                { 8, ARG_STRUCT, 0, true },
                { 52, ARG_STRUCT, 0, true } },
    .rettype = RET_INT
};

DisasmTrace ShipManager_OnLoop_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "ACH_TOUGH_SHIP",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { I_PUSH, .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 6, .imax = 12 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 1, .imax = 6 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ESP } },
                .argstr = { 0, "reactor" } },
             { DT_OP(SKIP), .imin = 300, .imax = 650 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL ShipSystem::GetExploded
              { I_TEST, .args = { ARG_REG, ARG_REG }, .args = { { REG_AL }, { REG_AL } } },
             { DT_OP(SKIP), .imin = 1, .imax = 6, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(ShipManager_DamageHull) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_OnLoop),   // DT_OUT_SYM1
              &SYM(ShipSystem_GetExploded) }
};

Symbol SYM(ShipManager_OnLoop) = {
    SYMNAME("ShipManager::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_OnLoop_trace }, { 0 } }
};
FuncInfo FUNCINFO(ShipManager_OnLoop) = { .nargs   = 1,
                                          .stdcall = true,
                                          .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                          .rettype = RET_VOID };
