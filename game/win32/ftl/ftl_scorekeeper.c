#include "ftl/capp.h"
#include "ftl/globals.h"
#include "ftl/scorekeeper.h"
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
             { DT_OP(FINISH) } },
    .out  = { &SYM(ScoreKeeper_Save),           // SYM1
              &SYM(TutorialManager_Tutorial),   // SYM2
              &SYM(TutorialManager_Running),    // SYM3
              &SYM(Settings_difficulty) }
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
