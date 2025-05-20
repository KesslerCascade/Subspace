#include "ftl/capp.h"
#include "ftl/scorekeeper.h"
#include "hook/disasmtrace.h"

DisasmTrace ScoreKeeper_Save_trace = {
    .c    = DTRACE_STRREFS,
    .mod  = DTRACE_MOD_FUNCSTART,
    .cstr = "Something went wrong in saving the profile!\n",
    .ops  = { { DT_OP(NOOP), .outip = DT_OUT_SYM1 },
             { I_PUSH },
             { DT_OP(SKIP), .imin = 7, .imax = 14 },
             {
                 I_MOV,
                  .argf = { ARG_REG },
                  .args = { { REG_ECX } }
                 //             .argout = { 0, DT_OUT_SYM2 }
                 // TutorialManager::Tutorial
             }, { DT_OP(SKIP), .imin = 1, .imax = 5 },
             {
                 I_CALL
                 // .argout = { 0, DT_OUT_SYM3 }
                 // TutorialManager::Running
             }, { DT_OP(SKIP), .imin = 1, .imax = 5 },
             {
                 I_MOV,
                  .argcap = { DT_CAPTURE1 }
                 //.argout = {DT_OUT_SYM4}
                 // settings.difficulty
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
    .out  = { &SYM(ScoreKeeper_Save) }
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
