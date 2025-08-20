#include "ftl/capp.h"
#include "ftl/commandgui.h"
#include "ftl/filehelper.h"
#include "ftl/globals.h"
#include "ftl/scorekeeper.h"
#include "ftl/settings.h"
#include "ftl/tutorialmanager.h"
#include "ftl/worldmanager.h"
#include "hook/disasmtrace.h"

Symbol SYM(ScoreKeeper_Keeper) = {
    SYMNAME("ScoreKeeper::Keeper"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper6KeeperE" },
             { 0 } }
};

DisasmTrace ScoreKeeper_Save_trace = {
    .c    = DTRACE_STRREFS,
    .mod  = DTRACE_MOD_FUNCSTART,
    .cstr = "Something went wrong in saving the profile!\n",
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { I_PUSH },
             { DT_OP(SKIP), .imin = 7, .imax = 14 },
             {
                 I_MOV,
                  .argf   = { ARG_REG },
                  .args   = { { REG_ECX } },
                  .argout = { 0, DT_OUT_SYM2 }   // TutorialManager::Tutorial
             }, { DT_OP(SKIP), .imin = 1, .imax = 5 },
             {
                 I_CALL,
                  .argout = { DT_OUT_SYM3 }   // TutorialManager::Running
             }, { DT_OP(SKIP), .imin = 1, .imax = 5 },
             {
                 I_MOV,
                  .argcap = { DT_CAPTURE1 },
                  .argout = { 0, DT_OUT_SYM4 }   // settings.difficulty
             }, { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CMP,   // difficulty == 1
                .argf   = { ARG_MATCH, ARG_ADDR },
                .argcap = { DT_MATCH1 },
                .args   = { { 0 }, { .addr = 1 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 4 },
             { I_CMP,   // difficulty == 2
                .argf   = { ARG_MATCH, ARG_ADDR },
                .argcap = { DT_MATCH1 },
                .args   = { { 0 }, { .addr = 2 } } },
             { DT_OP(SKIP), .imin = 9, .imax = 16 },
             { I_CVTSI2SS },
             { DT_OP(SKIP), .imin = 2, .imax = 9 },
             { I_CALL, .argout = { DT_OUT_SYM5 } },   // CALL Settings::GetDlcEnabled
              { DT_OP(SKIP), .imin = 250, .imax = 1000 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(FileHelper_closeBinaryFile) } },
             { DT_OP(SKIP), .imin = 2, .imax = 8, .flow = DT_FLOW_JMP_BOTH },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(FileHelper_fileExists) } },
             { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_LEA, .argcap = { 0, DT_CAPTURE2 } },   // temp filename
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL, .argout = { DT_OUT_SYM6 } },     // CALL FileHelper::deleteFile
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_LEA, .argf = { 0, ARG_MATCH }, .argcap = { 0, DT_MATCH2 } },   //
              { DT_OP(SKIP), .imin = 0, .imax = 7 },
             { I_CALL, .argout = { DT_OUT_SYM7 } },   // CALL FileHelper::renameFile
              { DT_OP(FINISH) } },
    .out  = { &SYM(ScoreKeeper_Save),                  // SYM1
              &SYM(TutorialManager_Tutorial),          // SYM2
              &SYM(TutorialManager_Running),           // SYM3
              &SYM(Settings_difficulty),               // SYM4
              &SYM(Settings_GetDlcEnabled),
             &SYM(FileHelper_deleteFile),             // SYM6
              &SYM(FileHelper_renameFile) }
};

INITWRAP(ScoreKeeper_Save);
Symbol SYM(ScoreKeeper_Save) = {
    SYMNAME("ScoreKeeper::Save"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ScoreKeeper_Save_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper4SaveEb" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_Save) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

INITWRAP(ScoreKeeper_SetVictory);
Symbol SYM(ScoreKeeper_SetVictory) = {
    SYMNAME("ScoreKeeper::SetVictory"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper10SetVictoryEb" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_SetVictory) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

INITWRAP(ScoreKeeper_SetSector);
Symbol SYM(ScoreKeeper_SetSector) = {
    SYMNAME("ScoreKeeper::SetSector"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_Restart_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper9SetSectorEi" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_SetSector) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } }
};

INITWRAP(ScoreKeeper_Reset);
Symbol SYM(ScoreKeeper_Reset) = {
    SYMNAME("ScoreKeeper:Reset"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateNewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper5ResetEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_Reset) = { .nargs   = 1,
                                         .stdcall = true,
                                         .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(ScoreKeeper_AddDefeatedShips);
Symbol SYM(ScoreKeeper_AddDefeatedShips) = {
    SYMNAME("ScoreKeeper::AddDefeatedShips"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_OnLoop_Mantis_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper16AddDefeatedShipsEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_AddDefeatedShips) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(ScoreKeeper_AddCrew);
Symbol SYM(ScoreKeeper_AddCrew) = {
    SYMNAME("ScoreKeeper::AddCrew"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateNewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper7AddCrewEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_AddCrew) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_VOID };

INITWRAP(ScoreKeeper_AddExploredLocations);
Symbol SYM(ScoreKeeper_AddExploredLocations) = {
    SYMNAME("ScoreKeeper::AddExploredLocations"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_CreateLocation_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper20AddExploredLocationsEv" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_AddExploredLocations) = {
    .nargs   = 1,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false } },
    .rettype = RET_VOID
};

INITWRAP(ScoreKeeper_AddScrapCollected);
Symbol SYM(ScoreKeeper_AddScrapCollected) = {
    SYMNAME("ScoreKeeper::AddScrapCollected"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &WorldManager_ModifyResources_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper17AddScrapCollectedEi" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_AddScrapCollected) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};

INITWRAP(ScoreKeeper_UnlockShip);
Symbol SYM(ScoreKeeper_UnlockShip) = {
    SYMNAME("ScoreKeeper::UnlockShip"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_RunCommand_SHIP_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN11ScoreKeeper10UnlockShipEiibb" },
             { 0 } }
};
FuncInfo FUNCINFO(ScoreKeeper_UnlockShip) = {
    .nargs   = 5,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_INT, 0, true },
                { 4, ARG_INT, 0, true },
                { 4, ARG_INT, 0, true },
                { 4, ARG_INT, 0, true } },
    .rettype = RET_VOID
};
