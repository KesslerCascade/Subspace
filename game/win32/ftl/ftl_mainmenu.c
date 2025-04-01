#include "ftl/capp.h"
#include "ftl/mainmenu.h"
#include "ftl/misc.h"
#include "hook/disasmtrace.h"

Symbol SYM(MainMenu_OnRender) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnRender_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN8MainMenu8OnRenderEv" },
             { 0 } }
};

// This really sucks but I haven't found a better place to get the version number than what's
// hardcoded in the main manu and spread all over the place.
DisasmTrace MainMenu_OnRender_trace = {
    .c    = DTRACE_ADDR,
    .csym = &SYM(MainMenu_OnRender),
    .ops  = { { DT_OP(SKIP), .imin = 100, .imax = 500 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } }, .argout = { 0, DT_OUT_SYM1 } },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = &SYM(basicstring_from_int) },
             { DT_OP(SKIP), .imin = 50, .imax = 300 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } }, .argout = { 0, DT_OUT_SYM2 } },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = &SYM(basicstring_from_int) },
             { DT_OP(SKIP), .imin = 50, .imax = 300 },
             { I_MOV, .argf = { ARG_REG }, .args = { { REG_ESP } }, .argout = { 0, DT_OUT_SYM3 } },
             { I_CALL, .argf = { ARG_MATCH }, .argsym = &SYM(basicstring_from_int) },
             { DT_OP(FINISH) } },
    .out  = { &SYM(version_rev), &SYM(version_minor), &SYM(version_major) }
};
