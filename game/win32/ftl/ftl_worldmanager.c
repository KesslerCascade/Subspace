#include "ftl/achievementtracker.h"
#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "ftl/completeship.h"
#include "ftl/globals.h"
#include "ftl/misc.h"
#include "ftl/scorekeeper.h"
#include "ftl/shipmanager.h"
#include "ftl/starmap.h"
#include "ftl/tutorialmanager.h"
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

INITWRAP(WorldManager_LoadGame);
Symbol SYM(WorldManager_LoadGame) = {
    SYMNAME("WorldManager::LoadGame"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_menu },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN12WorldManager8LoadGameENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_LoadGame) = {
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
             { DT_OP(SKIP),
                .imin = 0,   // this could be a JNZ or JZ depending on branch
                .imax = 1,   // ordering
                .flow = DT_FLOW_JMP_BOTH },
             { I_MOV },
             { DT_OP(SKIP), .imin = 14, .imax = 33 },
             { I_IMUL,
                .argf = { 0, 0, ARG_ADDR },
                .args = { { 0 }, { 0 }, { .disp = -0x55555555 } } },
             { DT_OP(SKIP), .imin = 1, .imax = 3 },
             { I_CMP, .argout = { DT_OUT_SYM4 } },
             { DT_OP(SKIP), .imin = 1, .imax = 3 },
             { I_CMOVLE },
             { DT_OP(SKIP), .imin = 1, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },          // CALL CompleteShip::OnInit
              { DT_OP(FINISH) } },
    .out  = { &SYM(WorldManager_CreateShip),                  // DT_OUT_SYM1
              &SYM(WorldManager_bossShip_offset),             // DT_OUT_SYM2
              0,                                              // for BossShip::StartStage
              &SYM(WorldManager_starMap_worldLevel_offset),   // DT_OUT_SYM5
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

DisasmTrace WorldManager_OnInit_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(WorldManager_OnInit),
    .ops  = { { DT_OP(SKIP), .imin = 7, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // capture register used for 'this'
              { DT_OP(SKIP), .imin = 6, .imax = 12 },
             {
                 I_MOV,
                  .argf   = { ARG_REG },
                  .argcap = { DT_MATCH1 }   // store commandgui pointer
             }, { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL CommandGui::OnInit
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, DT_MATCH1 },
                .argout = { 0, DT_OUT_SYM2 } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL CommandGui::LinkMap
              { DT_OP(FINISH) } },
    .out  = { &SYM(CommandGui_OnInit), &SYM(WorldManager_starMap_offset), &SYM(CommandGui_LinkMap) }
};

Symbol SYM(WorldManager_starMap_offset) = {
    SYMNAME("WorldManager->starMap"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnInit_trace }, { 0 } }
};

Symbol SYM(WorldManager_starMap_worldLevel_offset) = {
    SYMNAME("WorldManager->starMap.worldLevel"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateShip_trace }, { 0 } }
};

INITWRAP(WorldManager_Restart);
Symbol SYM(WorldManager_Restart) = {
    SYMNAME("WorldManager::Restart"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s0 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager7RestartEv" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_Restart) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

DisasmTrace WorldManager_Restart_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(WorldManager_Restart),
    .ops  = { { DT_OP(SKIP), .imin = 7, .imax = 16 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ClearLocation(this)
              { DT_OP(SKIP), .imin = 11, .imax = 18 },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_CAPTURE1 },
                .argout = { 0, DT_OUT_SYM2 } },        // static ScoreKeeper instance
              { I_MOV, .argf = { ARG_REG, ARG_ADDR }, .args = { { REG_ESP }, { .addr = 0 } } },
             { I_CALL, .argout = { DT_OUT_SYM3 } },   // CALL ScoreKeeper::SetVictory
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 } },          // static ScoreKeeper instance
              { I_MOV, .argf = { ARG_REG, ARG_ADDR }, .args = { { REG_ESP }, { .addr = 1 } } },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL ScoreKeeper::SetSector
              { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .args = { ARG_ADDR }, .argsym = { &SYM(CommandGui_Restart) } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },   // CALL CreateNewGame(this)
              { DT_OP(FINISH) } },
    .out  = { &SYM(WorldManager_ClearLocation),        // DT_OYT_SYM1
              &SYM(ScoreKeeper_Keeper),                // DT_OUT_SYM2
              &SYM(ScoreKeeper_SetVictory),            // DT_OUT_SYM3
              &SYM(ScoreKeeper_SetSector),             // DT_OUT_SYM4
              &SYM(WorldManager_CreateNewGame) }
};

INITWRAP(WorldManager_CreateNewGame);
Symbol SYM(WorldManager_CreateNewGame) = {
    SYMNAME("WorldManager::CreateNewGame"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager13CreateNewGameEv" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_CreateNewGame) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

DisasmTrace WorldManager_CreateNewGame_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(WorldManager_CreateNewGame),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 12 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // saved this pointer
              { DT_OP(SKIP), .imin = 32, .imax = 53 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(ScoreKeeper_Keeper) } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL ScoreKeeper::Reset
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(AchievementTracker_Tracker) } },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL AchievementTracker::Tracker
              { DT_OP(SKIP), .imin = 14, .imax = 24 },
             { I_MOV,
                .argf   = { ARG_PTRSIZE, ARG_MATCH },
                .args   = { { .ptrsize = 1 }, { .base = REG_UNDEF, .idx = REG_UNDEF, .addr = 0 } },
                .argout = { DT_OUT_SYM3 } },
             { DT_OP(SKIP), .imin = 0, .imax = 8 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(TutorialManager_Running) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_LEA,
                .argf   = { ARG_REG, ARG_ADDR },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(WorldManager_starMap_offset) } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },   // CALL StarMap::NewGame
              { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argcap = { 0, DT_MATCH1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ScoreKeeper_Reset),               // DT_OUT_SYM1
              &SYM(AchievementTracker_ResetFlags),   // DT_OUT_SYM2
              &SYM(RNG_useSysRand),                  // DT_OUT_SYM3
              &SYM(StarMap_NewGame),                 // DT_OUT_SYM4
              &SYM(WorldManager_CreateLocation) }
};

INITWRAP(WorldManager_ClearLocation);
Symbol SYM(WorldManager_ClearLocation) = {
    SYMNAME("WorldManager::ClearLocation"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12WorldManager13ClearLocationEv" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_ClearLocation) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(WorldManager_CreateLocation);
Symbol SYM(WorldManager_CreateLocation) = {
    SYMNAME("WorldManager::CreateLocation"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateNewGame_trace },
             { .type = SYMBOL_FIND_EXPORT,
                .name = "_ZN12WorldManager14CreateLocationEP8Location" },
             { 0 } }
};
FuncInfo FUNCINFO(WorldManager_CreateLocation) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};
