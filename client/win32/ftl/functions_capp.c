#include "ftl/functions_capp.h"
#include "hook/disasmfind.h"

// CApp::OnExecute is called from sil_main
INITWRAP(CApp_OnExecute);
DisasmFind CApp_OnExecute_Disasm = {
    .candidates = DISASM_SEARCH_STRREF,
    .cstr       = "Starting up\n",
    .ops        = { { .inst = I_MOV },
                   { .inst = I_CALL },
                   { .inst = I_LEA, .argfilter = { ARG_REG }, .args = { { .base = REG_ECX } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },   // result is arg1 of the CALL
                    { DISASM_FINISH } }
};
Symbol SYM(CApp_OnExecute) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnExecute_Disasm },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN4CApp9OnExecuteEv" },
             { 0 } }
};
FuncInfo FUNCINFO(CApp_OnExecute) = { .nargs = 1, .args = { { 4, ARG_PTR, REG_ECX, false } } };
