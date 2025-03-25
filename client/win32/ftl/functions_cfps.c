#include "ftl/functions_misc.h"

#include "ftl/functions_cfps.h"
#include "hook/disasmfind.h"

// WorldManager construction is a convenient place to find the address of 'operator new' because
// it's called immediately after a specific log message.
DisasmFind CFPS_FPSControl_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Running Game!\n",
    .ops        = { { DISASM_SKIP, .imin = 6, .imax = 15 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
                   { DISASM_SKIP, .imin = 2, .imax = 6 },
                   { .inst = I_CALL, .argfilter = { ARG_REG }, .args = { { .base = REG_EAX } } },
                   { DISASM_SKIP, .imin = 0, .imax = 1 },   // Steam version calls the store wrapper here
                    { .inst      = I_MOV,
                      .argfilter = { ARG_REG },
                      .args      = { { .base = REG_ECX } },
                      .mark      = { MARK_ARG2 } },
                   { DISASM_FINISH } }
};
Symbol SYM(CFPS_FPSControl) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CFPS_FPSControl_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS10FPSControlE" },
             { 0 } }
};

DisasmFind CFPS_GetTime_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Running Game!\n",
    .ops        = { { DISASM_SKIP, .imin = 6, .imax = 15 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
                   { DISASM_SKIP, .imin = 2, .imax = 6 },
                   { .inst = I_CALL, .argfilter = { ARG_REG }, .args = { { .base = REG_EAX } } },
                   { DISASM_SKIP, .imin = 0, .imax = 1 },   // Steam version calls the store wrapper here
                    { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },
                   { DISASM_FINISH } }
};
Symbol SYM(CFPS_GetTime) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CFPS_GetTime_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CFPS7GetTimeEv" },
             { 0 } }
};
