#include "ftl/functions_misc.h"
#include "hook/disasmfind.h"

// WorldManager construction is a convenient place to find the address of 'operator new' because
// it's called immediately after a specific log message.
DisasmFind operator_new_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Generating world...\n",
    .ops        = { { DISASM_SKIP, .imin = 1, .imax = 8 },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ESP } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // result is arg1 of the CALL
                    { DISASM_FINISH } }
};
Symbol SYM(operator_new) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &operator_new_Disasm }, { 0 } }
};

DisasmFind operator_delete_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Starting audio library...\n",
    .ops        = { { DISASM_SKIP, .imin = 12, .imax = 18 },
                   { .inst = I_JZ },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { REG_ESP } },
                   { DISASM_SKIP, .imin = 0, .imax = 1 },
                   {
                        .inst = I_CALL,
                        .mark = MARK_ARG1,
             }, { DISASM_FINISH } }
};
Symbol SYM(operator_delete) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &operator_delete_Disasm }, { 0 } }
};
