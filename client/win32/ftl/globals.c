#include "ftl/globals.h"
#include "hook/disasmfind.h"

// WorldManager construction is a convenient place to find the address of 'operator new' because
// it's called immediately after a specific log message.
DisasmFind opt_framelimit_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "framelimit=",
    .ops        = { { DISASM_SKIP, .imin = 1, .imax = 4 },
                   { .inst = I_CALL },
                   { .inst      = I_MOVZX,
                      .argfilter = { ARG_REG },
                      .args      = { { .base = REG_EAX } },
                      .mark      = MARK_ARG2 },
                   { DISASM_FINISH } }
};
Symbol SYM(opt_framelimit) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &opt_framelimit_Disasm }, { 0 } }
};
