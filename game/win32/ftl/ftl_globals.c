#include "ftl/capp.h"
#include "ftl/cfps.h"
#include "ftl/globals.h"
#include "ftl/scorekeeper.h"
#include "hook/disasmtrace.h"

// Move to Settings module if we ever hook it
DisasmTrace Settings_SaveSettings_trace_1 = {
    .c    = DTRACE_STRREFS,
    .cstr = "framelimit=",
    .ops  = { { DT_OP(SKIP), .imin = 1, .imax = 4 },
             { I_CALL },
             { I_MOVZX,
                .argf   = { ARG_IGNORE, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 1 } },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(opt_framelimit) }
};

// some versions order the instructions a little differently
DisasmTrace Settings_SaveSettings_trace_2 = {
    .c    = DTRACE_STRREFS,
    .cstr = "vsync=",
    .ops  = { { DT_OP(SKIP), .imin = 9, .imax = 14 },
             { I_CALL },
             { I_MOVZX,
                .argf   = { ARG_IGNORE, ARG_PTRSIZE },
                .args   = { { 0 }, { .ptrsize = 1 } },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 2 },
             { I_MOV, .argf = { 0, ARG_MATCH }, .argstr = { 0, "framelimit=" } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(opt_framelimit) }
};

Symbol SYM(opt_framelimit) = {
    SYMNAME("opt_framelimit"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &Settings_SaveSettings_trace_1 },
             { .type = SYMBOL_FIND_DISASM, .disasm = &Settings_SaveSettings_trace_2 },
             { 0 } }
};

Symbol SYM(Globals_Library) = {
    SYMNAME("Globals::Library"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v1_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v2_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN7Globals7LibraryE" },
             { 0 } }
};

Symbol SYM(Globals_Library_currentLanguage) = {
    SYMNAME("Globals::Library.currentLanguage"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v2_trace },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnInit_v1_trace },
             { 0 } }
};

DisasmTrace TargetTickTime_trace = {
    .c    = DTRACE_CALLS,
    .csym = &SYM(CFPS_GetTime),
    .ops  = { { I_CALL },
             { I_FSTP, .argcap = { DT_CAPTURE1 } },
             { DT_OP(SKIP), .imin = 0, .imax = 5 },
             { I_MOVSD, .argcap = { 0, DT_CAPTURE2 } },
             { DT_OP(SKIP), .imin = 9, .imax = 18 },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = { &SYM(CFPS_GetTime) } },
             { I_FSTP, .argf = { ARG_MATCH }, .argcap = { DT_MATCH1 } },
             { I_MOVSD,
                .argf   = { 0, ARG_MATCH },
                .argcap = { 0, DT_MATCH2 },
                .argout = { 0, DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(TargetFrameTimeMS) }
};

Symbol SYM(TargetFrameTimeMS) = {
    SYMNAME("TargetFrameTimeMS"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &TargetTickTime_trace }, { 0 } }
};

Symbol SYM(ResourceControl_GlobalResources) = {
    SYMNAME("ResourceControl::GlobalResources"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN15ResourceControl15GlobalResourcesE" },
             { 0 } }
};

Symbol SYM(Settings_difficulty) = {
    SYMNAME("Settings::difficulty"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &ScoreKeeper_Save_trace }, { 0 } }
};
