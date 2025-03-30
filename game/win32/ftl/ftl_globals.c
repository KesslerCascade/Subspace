#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "hook/disasmtrace.h"

// Move to Settings module if we ever hook it
DisasmTrace Settings_SaveSettings_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "framelimit=",
    .ops  = { { DT_OP(SKIP), .imin = 1, .imax = 4 },
             { I_CALL },
             { I_MOVZX,
                .argf   = { ARG_REG },
                .args   = { { REG_EAX } },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(opt_framelimit) }
};

Symbol SYM(opt_framelimit) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Settings_SaveSettings_trace }, { 0 } }
};

Symbol SYM(Globals_Library) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v1_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v2_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7Globals7LibraryE" },
             { 0 } }
};

Symbol SYM(Globals_Library_currentLanguage) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v2_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v1_trace },
             { 0 } }
};

DisasmTrace TargetTickTime_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(CFPS_GetTime),
    .ops  = { { I_CALL },
             { I_FSTP, .argcap = { CT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOVSD, .argcap = { 0, CT_CAPTURE2 } },
             { DT_OP(SKIP), .imin = 9, .imax = 18 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(CFPS_GetTime) } },
             { I_FSTP, .argf = { ARG_MATCH }, .argcap = { CT_MATCH1 } },
             { I_MOVSD,
                .argf   = { 0, ARG_MATCH },
                .argcap = { 0, CT_MATCH2 },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(TargetFrameTimeMS) }
};

Symbol SYM(TargetFrameTimeMS) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &TargetTickTime_trace }, { 0 } }
};
