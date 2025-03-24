#include "ftl/functions_capp.h"
#include "hook/disasmfind.h"

INITWRAP(CApp_OnExecute);
DisasmFind CApp_OnExecute_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Starting up\n",
    .ops        = { { .type = DISASM_INSTR, .inst = I_MOV },
                   { .type = DISASM_INSTR, .inst = I_CALL },
                   { .type      = DISASM_INSTR,
                      .inst      = I_LEA,
                      .argfilter = { ARG_MATCH },
                      .args      = { { .base = REG_ECX, .idx = REG_UNDEF } } },
                   { .type = DISASM_MARK, .argfilter = { ARG_MARK, 0, 0 } },
                   { .type = DISASM_INSTR, .inst = I_CALL },
                   { 0 } }
};
Symbol SYM(CApp_OnExecute) = {
    .find = { { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp9OnExecuteEv" },
             { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_Disasm },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnExecute) = { .nargs = 1, .args = { { 4, ARG_PTR, REG_ECX, false } } };
