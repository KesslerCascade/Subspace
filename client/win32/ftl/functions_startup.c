#include "ftl/functions_startup.h"
#include "hook/disasmfind.h"

INITWRAP(sil_main);
DisasmFind sil_main_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "SIL",
    .ops        = { { DISASM_SKIP, .imin = 1, .imax = 5 },
                   { DISASM_CALL },      // follow sil__main call
                    { DISASM_SKIP, .imin = 3, .imax = 10 },
                   { .inst = I_CALL },   // thread_init
                    { .inst = I_TEST },
                   { .inst = I_JZ },
                   { .inst = I_CALL },   // time_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // sys_file_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // graphixcs_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // input_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // resource_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // sound_init
                    { DISASM_SKIP, .imin = 0, .imax = 3 },
                   { .inst = I_CALL },   // userdata_init
                    { DISASM_SKIP, .imin = 3, .imax = 12 },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_MATCH },
                      .args      = { { .base = REG_ESP, .idx = REG_UNDEF, .addr = 0 } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // sil_main
                    { DISASM_FINISH } }
};
Symbol SYM(sil_main) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &sil_main_Disasm },
             //{ .type = SYMBOL_FIND_EXPORT, .name = "sil_main" },
              { 0 } }
};
FuncInfo FUNCINFO(sil_main) = { .nargs = 2, .purecdecl = true };
