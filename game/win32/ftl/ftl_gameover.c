#include "ftl/cfps.h"
#include "ftl/commandgui.h"
#include "ftl/gameover.h"
#include "ftl/soundcontrol.h"
#include "hook/disasmtrace.h"

Symbol SYM(GameOver_SetVictory) = {
    SYMNAME("GameOver::SetVictory"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8GameOver10SetVictoryEbNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEESt6vectorIS5_SaIS5_EE" },
             { 0 } }
};
FuncInfo FUNCINFO(GameOver_SetVictory) = {
    .nargs   = 4,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false },
                { 4, ARG_INT, 0, true },
                { 4, ARG_PTR, 0, true },
                { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};

Symbol SYM(GameOver_OpenText) = {
    SYMNAME("GameOver::OpenText"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_CheckGameOver_trace_2 },
             { .type = SYMBOL_FIND_EXPORT,
                .name =
                    "_ZN8GameOver8OpenTextENSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE" },
             { 0 } }
};
FuncInfo FUNCINFO(GameOver_OpenText) = {
    .nargs   = 2,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
    .rettype = RET_VOID
};

Symbol SYM(GameOver_OnLoop) = {
    SYMNAME("GameOver::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_OnLoop_GameOverLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8GameOver6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(GameOver_OnLoop) = { .nargs   = 1,
                                       .stdcall = true,
                                       .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                       .rettype = RET_VOID };

DisasmTrace GameOver_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(GameOver_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 12 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // this pointer,
              { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CALL, .argf = { ARG_ADDR }, .argsym = { &SYM(CFPS_GetSpeedFactor) } },
             { DT_OP(SKIP), .imin = 10, .imax = 50 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x6c746974 } } },   // 'titl'
              { DT_OP(SKIP), .imin = 7, .imax = 16, .flow = DT_FLOW_JMP_BOTH },
             { I_MOV,
                .argf   = { ARG_REG },
                .args   = { { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 } },        // this = SoundControl::Sounds
              { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_CALL, .argout = { DT_OUT_SYM2 } },   // CALL SoundControl::StartPlaylist
              { DT_OP(FINISH) } },
    .out  = { &SYM(SoundControl_Sounds),               // DT_OUT_SYM1
              &SYM(SoundControl_StartPlaylist) }
};
