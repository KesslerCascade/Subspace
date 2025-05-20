#include "ftl/misc.h"

#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/mousecontrol.h"
#include "hook/disasmtrace.h"

// WorldManager construction is a convenient place to find the address of 'operator new' because
// it's called immediately after a specific log message.
Symbol SYM(CFPS_FPSControl) = {
    SYMNAME("CFPS::FPSControl"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_rungame_trace_1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_rungame_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS10FPSControlE" },
             { 0 } }
};

INITWRAP(CFPS_GetTime);
Symbol SYM(CFPS_GetTime) = {
    SYMNAME("CFPS::GetTime"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_rungame_trace_1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_rungame_trace_2 },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS7GetTimeEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CFPS_GetTime) = { .nargs   = 1,
                                    .stdcall = true,
                                    .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                    .rettype = RET_FLOAT64 };

INITWRAP(CFPS_OnLoop);
Symbol SYM(CFPS_OnLoop) = {
    SYMNAME("CFPS::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS6OnLoopEv" },
             { 0 } }
};
DECLFUNC(CFPS_OnLoop);
FuncInfo FUNCINFO(CFPS_OnLoop) = { .nargs   = 1,
                                   .stdcall = true,
                                   .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                   .rettype = RET_VOID };

INITWRAP(CFPS_GetSpeedFactor);
Symbol SYM(CFPS_GetSpeedFactor) = {
    SYMNAME("CFPS::GetSpeedFactor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &MouseControl_OnLoop_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS14GetSpeedFactorEv" },
             { 0 } }
};
DECLFUNC(CFPS_GetSpeedFactor);
FuncInfo FUNCINFO(CFPS_GetSpeedFactor) = { .nargs   = 1,
                                           .stdcall = true,
                                           .args    = { { 4, ARG_PTR, REG_ECX, false } },
                                           .rettype = RET_FLOAT32 };

DisasmTrace CFPS_GetSpeedFactor_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CFPS_GetSpeedFactor),
    .ops  = { { I_SUB },
             { I_MOVSS,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argout = { 0, DT_OUT_SYM1 } },   // MOVSS [ECX+?]
              { DT_OP(FINISH) } },
    .out  = { &SYM(CFPS_SpeedFactor_offset) }
};

DisasmTrace CFPS_OnLoop_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(CFPS_OnLoop),
    .ops  = { { DT_OP(SKIP), .imin = 5, .imax = 15 },
             { I_MOV,
                .argf   = { 0, ARG_REG },
                .args   = { { 0 }, { REG_ECX } },
                .argcap = { DT_CAPTURE1 } },   // get reg 'this' ponter is stashed in
              { DT_OP(SKIP), .imin = 17, .imax = 28 },
             { I_UCOMISD },
             { I_JA, DT_OP(JMP) },   // follow JA
              { DT_OP(SKIP), .imin = 3, .imax = 8 },
             { I_MOV,
                .argcap = { DT_CAPTURE2, DT_CAPTURE3 } },   // sequence is retrieve frame counter,
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_MOV,
                .argf = { ARG_MATCH, ARG_MATCH },
                .args = { { 0 }, { .base = REG_UNDEF, .idx = REG_UNDEF, .addr = 0, .ptrsize = 4 } },
                .argcap = { DT_MATCH3 } },   // zero it
              { I_MOV,
                .argf   = { ARG_REG, ARG_MATCH },
                .argcap = { DT_MATCH1, DT_MATCH2 },
                .argout = { DT_OUT_SYM1 } },   // then store it relative to 'this'
              { DT_OP(FINISH) } },
    .out  = { &SYM(CFPS_fps_offset) }
};

Symbol SYM(CFPS_SpeedFactor_offset) = {
    SYMNAME("CFPS->SpeedFactor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CFPS_GetSpeedFactor_trace }, { 0 } }
};

Symbol SYM(CFPS_fps_offset) = {
    SYMNAME("CFPS->fps"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CFPS_OnLoop_trace }, { 0 } }
};
