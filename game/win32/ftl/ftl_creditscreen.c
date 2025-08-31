#include "ftl/creditscreen.h"
#include "ftl/mousecontrol.h"
#include "hook/disasmtrace.h"

DisasmTrace CreditScreen_OnRender_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(MouseControl_Reset),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 20 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x72617473 } } },   // star
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf = { 0, ARG_ADDR },
                .args = { { 0 }, { .addr = 0x67622f73 } } },   // s/bg
              { DT_OP(SKIP), .imin = 15, .imax = 25 },
             { I_MOVSS,
                .argf   = { 0, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 4 } },
                .argout = { 0, DT_OUT_SYM1 } },
             { I_UCOMISS },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CreditScreen_pausing_offset) }
};

Symbol SYM(CreditScreen_pausing_offset) = {
    SYMNAME("CreditScreen->pausing"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CreditScreen_OnRender_trace }, { 0 } }
};
