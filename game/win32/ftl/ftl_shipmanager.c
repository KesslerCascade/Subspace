#include "ftl/commandgui.h"
#include "ftl/ship.h"
#include "ftl/shipmanager.h"
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

Symbol SYM(ShipManager_myBlueprint_offset) = {
    SYMNAME("ShipManager->myBlueprint"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ShipManager_OnInit_trace }, { 0 } }
};

// offset within the embedded ShipBlueprint structure
Symbol SYM(ShipManager_myBlueprint_name_offset) = {
    SYMNAME("ShipManager->myBlueprint->name"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_StartGame_trace }, { 0 } }
};
