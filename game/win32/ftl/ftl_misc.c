#include "ftl/capp.h"
#include "ftl/mainmenu.h"
#include "ftl/misc.h"
#include "ftl/starmap.h"
#include "hook/disasmtrace.h"

Symbol SYM(operator_new) = {
    SYMNAME("operator new"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

Symbol SYM(operator_delete) = {
    SYMNAME("operator delete"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_audio_trace }, { 0 } }
};

Symbol SYM(ftl_log) = {
    SYMNAME("ftl_log"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_worldgen_trace }, { 0 } }
};

DisasmTrace basicstring_from_int_trace = {
    .c    = DTRACE_STRREFS,
    .cstr = "screen_x=",
    .ops  = { { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL },
             { DT_OP(SKIP), .imin = 0, .imax = 6 },
             { I_CALL, .argout = { DT_OUT_SYM1 } },
             { DT_OP(FINISH) } },
    .out  = { &SYM(basicstring_from_int) }
};

Symbol SYM(basicstring_from_int) = {
    SYMNAME("basicstring_from_int"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &basicstring_from_int_trace }, { 0 } }
};

Symbol SYM(version_major) = {
    SYMNAME("version_major"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &MainMenu_OnRender_trace }, { 0 } }
};

Symbol SYM(version_minor) = {
    SYMNAME("version_minor"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &MainMenu_OnRender_trace }, { 0 } }
};

Symbol SYM(version_rev) = {
    SYMNAME("version_rev"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &MainMenu_OnRender_trace }, { 0 } }
};

INITWRAP(random32);
Symbol SYM(random32) = {
    SYMNAME("random32"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_NewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "random32" },
             { 0 } }
};
FuncInfo FUNCINFO(random32) = { .nargs = 0, .purecdecl = true, .rettype = RET_INT };

INITWRAP(srandom32);
Symbol SYM(srandom32) = {
    SYMNAME("srandom32"),
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &StarMap_NewGame_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "srandom32" },
             { 0 } }
};
FuncInfo FUNCINFO(srandom32) = { .nargs     = 1,
                                 .purecdecl = true,
                                 .args      = { { 4, ARG_INT, 0, true } },
                                 .rettype   = RET_VOID };
