#include "ftl/capp.h"
#include "ftl/graphics/freetype.h"
#include "ftl/mainmenu.h"
#include "ftl/misc.h"
#include "hook/disasmtrace.h"

Symbol SYM(MainMenu_OnRender) = {
    SYMNAME("MainMenu::OnRender"),
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
             { DT_OP(SKIP), .imin = 0, .imax = 1000 },
             { I_MOV, .argf = { 0, ARG_ADDR }, .args = { { 0 }, { .addr = 0x44278000 } } },
             { DT_OP(SKIP), .imin = 0, .imax = 10 },
             { I_CALL, .argout = { DT_OUT_SYM4 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(version_rev),
             &SYM(version_minor),
             &SYM(version_major),
             &SYM(freetype_easy_measurePrintLines) }
};

INITWRAP(MainMenu_Open);
Symbol SYM(MainMenu_Open) = {
    SYMNAME("MainMenu::Open"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_trace_s5 },
             { .type = SYMBOL_FIND_IMPORT, .name = "_ZN8MainMenu4OpenEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MainMenu_Open) = { .nargs   = 1,
                                     .stdcall = true,
                                     .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(MainMenu_OnLoop);
Symbol SYM(MainMenu_OnLoop) = {
    SYMNAME("MainMenu::OnLoop"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_menu },
             { .type = SYMBOL_FIND_IMPORT, .name = "_ZN8MainMenu6OnLoopEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MainMenu_OnLoop) = { .nargs   = 1,
                                       .stdcall = true,
                                       .args    = { { 4, ARG_PTR, REG_ECX, false } } };

INITWRAP(MainMenu_Choice);
Symbol SYM(MainMenu_Choice) = {
    SYMNAME("MainMenu::Choice"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnLoop_menu },
             { .type = SYMBOL_FIND_IMPORT, .name = "_ZN8MainMenu6ChoiceEv" },
             { 0 } }
};
FuncInfo FUNCINFO(MainMenu_Choice) = { .nargs   = 1,
                                       .stdcall = true,
                                       .args    = { { 4, ARG_PTR, REG_ECX, false } } };
