#include "ftl/functions_cevent.h"
#include "ftl/functions_capp.h"
#include "hook/disasmtrace.h"

Symbol SYM(CEvent_callback) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

DisasmTrace CEvent_callback_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CEvent_callback),
    .ops  = { { DT_OP(SKIP), .imin = 8, .imax = 15 },
             { I_CMP, .argf = { ARG_REG }, .args = { { REG_EAX } } },
             { I_JNZ },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(CEvent_OnEvent_InputEvent) }
};

Symbol SYM(CEvent_OnEvent_InputEvent) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CEvent_callback_trace }, { 0 } }
};
FuncInfo FUNCINFO(CEvent_OnEvent_InputEvent) = {
    .nargs   = 2,
    .stdcall = true, // thiscall is essentially stdcall with ECX
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
};

DisasmTrace CEvent_OnEvent_InputEvent_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CEvent_OnEvent_InputEvent),
    .ops  = { { DT_OP(SKIP), .imin = 10, .imax = 18 },
             { I_SUB, .argf = { ARG_REG, ARG_ADDR }, .args = { { REG_EAX }, { .addr = 7 } } },
             { DT_OP(SKIP), .imin = 2, .imax = 7 },
             { DT_OP(JMPTBL), .val = 0 },   // follow switch() case 7 (SUB 7 above makes it entry 0)
              { I_CMP },
             { I_JNZ },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { CT_CAPTURE1 } },   // vtable being pulled out of instance pointer (ECX)
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .argcap = { 0, CT_MATCH1 },       // match the register the vtable is in
                .argout = { 0, DT_OUT_SYM1 } },   // and output the offset
              { DT_OP(FINISH) } },
    .out  = { &SYM(CApp_vtable_OnKeyDown_offset) }
};