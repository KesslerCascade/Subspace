#include "ftl/capp.h"
#include "ftl/completeship.h"
#include "ftl/globals.h"
#include "ftl/misc.h"
#include "ftl/shipmanager.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

// WorldManager is constructed in CApp:OnExecute
INITWRAP(WorldManager_ctor);
Symbol SYM(WorldManager_ctor) = {
    SYMNAME("WorldManager::ctor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManagerC1Ev" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_ctor) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } } };

Symbol SYM(WorldManager_OnInit) = {
    SYMNAME("WorldManager::OnInit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager6OnInitEv" },
             { 0 } }
};

INITWRAP(WorldManager_StartGame);
Symbol SYM(WorldManager_StartGame) = {
    SYMNAME("WorldManager::StartGame"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s1 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager9StartGameEP11ShipManager" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_StartGame) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } }
};

DisasmTrace WorldManager_CreateShip_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "SHIPS_REBEL",
    .mod  = DTRACE_MOD_FUNCSTART,
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { DT_OP(SKIP), .imin = 7, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CMP,                         // if (boss) from function params
                .argf = { ARG_ADDR, ARG_MATCH },
                .args = { { .disp = 4 }, { REG_UNDEF, .idx = REG_UNDEF, .addr = 0 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { DT_OP(LABEL), .val = 1 },      // come back to this branch later
              { DT_OP(JMP), .inst = I_JNZ },   // follow if(boss) branch

              { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argout = { 0, DT_OUT_SYM2 },   // bossShip_offset
                .argcap = { DT_CAPTURE2 } },
             { I_MOV,
                .argf   = { ARG_REG, ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH2 } },
             { I_CALL },                  // CALL BossShip::StartStage
              { DT_OP(GOTO), .val = 1 },   // go back to main branch
              { DT_OP(SKIP), .imin = 20, .imax = 36 },
             { I_TEST },
             { DT_OP(JMP) },
             { DT_OP(SKIP), .imin = 14, .imax = 27 },
             { I_IMUL,
                .argf = { 0, 0, ARG_ADDR },
                .args = { { 0 }, { 0 }, { .disp = -0x55555555 } } },
             { DT_OP(SKIP), .imin = 2, .imax = 6 },
             { I_CMOVLE },
             { DT_OP(SKIP), .imin = 1, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL CompleteShip::OnInit
              { DT_OP(FINISH) } },
    .out  = { &SYM(WorldManager_CreateShip),           // DT_OUT_SYM1
              &SYM(WorldManager_bossShip_offset),      // DT_OUT_SYM2
              0,                                       // for BossShip::StartStage
              &SYM(CompleteShip_OnInit) }
};

Symbol SYM(WorldManager_CreateShip) = {
    SYMNAME("WorldManager::CreateShip"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateShip_trace }, { 0 } }
};

Symbol SYM(WorldManager_bossShip_offset) = {
    SYMNAME("WorldManager->bossShip"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateShip_trace }, { 0 } }
};

DisasmTrace WorldManager_StartGame_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(WorldManager_StartGame),
    .ops  = { { DT_OP(SKIP), .imin = 45, .imax = 60 },
             { I_ADD, .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ESP } },
                .argsym = { 0, &SYM(Globals_Library) } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ShipManager_myBlueprint_name_offset) },
};
