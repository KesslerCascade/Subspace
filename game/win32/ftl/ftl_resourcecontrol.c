#include "ftl/capp.h"
#include "ftl/resourcecontrol.h"
#include "hook/disasmtrace.h"

INITWRAP(ResourceControl_RenderLoadingBar);
Symbol SYM(ResourceControl_RenderLoadingBar) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN15ResourceControl16RenderLoadingBarEff" },
             { 0 } }
};
FuncInfo FUNCINFO(ResourceControl_RenderLoadingBar) = {
    .nargs   = 3,
    .stdcall = true,
    .args = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_FLOAT, 0, true }, { 4, ARG_FLOAT, 0, true } }
};

DisasmTrace ResourceControl_RenderLoadingBar_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(ResourceControl_RenderLoadingBar),
    .ops  = { { DT_OP(SKIP), .imin = 6, .imax = 16 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { CT_CAPTURE1 } },   // self ptr
              { DT_OP(SKIP), .imin = 8, .imax = 20 },
             { I_CVTSI2SS,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, CT_MATCH1 },
                .argout = { 0, DT_OUT_SYM1 } },
             { I_CVTSI2SS,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, CT_MATCH1 },
                .argout = { 0, DT_OUT_SYM2 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(ResourceControl_preLoadProgress1_offset),
             &SYM(ResourceControl_preLoadProgress2_offset) }
};

Symbol SYM(ResourceControl_preLoadProgress1_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ResourceControl_RenderLoadingBar_trace },
             { 0 } }
};
Symbol SYM(ResourceControl_preLoadProgress2_offset) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ResourceControl_RenderLoadingBar_trace },
             { 0 } }
};
