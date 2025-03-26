#include "ftl/functions_cevent.h"
#include "hook/disasmfind.h"

DisasmFind CEvent_callback_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Running Game!\n",
    .ops        = { { .inst = I_MOV },
                   { .inst = I_CALL },
                   { .inst      = I_MOV,
                      .argfilter = { ARG_REG },
                      .args      = { { .base = REG_ESP } },
                      .mark      = MARK_ARG2 },
                   { DISASM_FINISH } }
};
Symbol SYM(CEvent_callback) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CEvent_callback_Disasm }, { 0 } }
};

DisasmFind CEvent_OnEvent_InputEvent_Disasm = {
    .candidates = DISASM_SEARCH_ADDR,
    .sym        = &SYM(CEvent_callback),
    .ops        = { { DISASM_SKIP, .imin = 8, .imax = 15 },
                   { .inst = I_CMP, .argfilter = { ARG_REG }, .args = { { .base = REG_EAX } } },
                   { .inst = I_JNZ },
                   { .inst = I_CALL, .mark = MARK_ARG1 },
                   { DISASM_FINISH } }
};
Symbol SYM(CEvent_OnEvent_InputEvent) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CEvent_OnEvent_InputEvent_Disasm }, { 0 } }
};
FuncInfo FUNCINFO(CEvent_OnEvent_InputEvent) = {
    .nargs   = 2,
    .stdcall = true, // thiscall is essentially stdcall with ECX
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_PTR, 0, true } },
};
