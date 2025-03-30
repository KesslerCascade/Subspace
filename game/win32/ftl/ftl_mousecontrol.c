#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "ftl/mousecontrol.h"
#include "hook/disasmtrace.h"

INITWRAP(MouseControl_OnRender);
Symbol SYM(MouseControl_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12MouseControl8OnRenderEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MouseControl_OnRender) = { .nargs   = 1,
                                             .stdcall = true,
                                             .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(MouseControl_OnLoop);
Symbol SYM(MouseControl_OnLoop) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN12MouseControl6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MouseControl_OnLoop) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } } };

DisasmTrace MouseControl_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(MouseControl_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 9 },
             { I_CMP, .argf = { ARG_REG }, .args = { { REG_ECX } } },
             { DT_OP(JMP) },   // follow the JE
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .args   = { { REG_ECX } },
                .argsym = { 0, &SYM(CFPS_FPSControl) } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // CALL CFPS::GetSpeedFactor
              { DT_OP(FINISH) } },
    .out  = { &SYM(CFPS_GetSpeedFactor) }
};
