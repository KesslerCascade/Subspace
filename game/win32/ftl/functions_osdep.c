#include "ftl/functions_osdep.h"
#include "ftl/functions_startup.h"

#include "hook/disasmtrace.h"

INITWRAP(WinMain);
Symbol SYM(WinMain) = {
    .find = { { .type = SYMBOL_FIND_EXPORT, .name = "WinMain@16" }, { 0 } }
};
FuncInfo FUNCINFO(WinMain) = {
    .nargs = 4,
    .stdcall = true,
    .args = {
        { 4, ARG_INT, 0, true },
        { 4, ARG_INT, 0, true },
        { 4, ARG_PTR, 0, true },
        { 4, ARG_INT, 0, true },
    }
};

// WinMain calls sil__main which calls sil_main
DisasmTrace WinMain_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "SIL",
    .ops  = { { DT_OP(SKIP), .imin = 1, .imax = 5 },
             { DT_OP(CALL) },   // follow sil__main call
              { DT_OP(SKIP), .imin = 3, .imax = 10 },
             { I_CALL },        // thread_init
              { I_TEST },
             { I_JZ },
             { I_CALL },   // time_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // sys_file_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // graphixcs_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // input_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // resource_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // sound_init
              { DT_OP(SKIP), .imin = 0, .imax = 3 },
             { I_CALL },   // userdata_init
              { DT_OP(SKIP), .imin = 3, .imax = 12 },
             { I_MOV, .argf = { ARG_MATCH }, .args = { { REG_ESP, .idx = REG_UNDEF, .addr = 0 } } },
             { I_CALL, .argout = { DT_OUT_SYM1 } },   // sil_main
              { DT_OP(FINISH) } },
    .out  = { &SYM(sil_main) }
};
