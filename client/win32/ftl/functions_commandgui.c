#include "ftl/functions_commandgui.h"
#include "ftl/functions_capp.h"
#include "ftl/functions_misc.h"
#include "hook/disasmfind.h"

// CApp::OnExecute is called from sil_main
INITWRAP(CommandGui_KeyDown);
DisasmFind CommandGui_KeyDown_Disasm = {
    .candidates = DISASM_SEARCH_ADDR,
    .sym        = &SYM(CApp_OnKeyDown),
    .ops        = { { DISASM_SKIP, .imin = 35, .imax = 45 },
                   { .inst = I_CALL, .argcap = { CT_CAPTURE0 } },
                   { DISASM_SKIP, .imin = 3, .imax = 10 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
                   { DISASM_SKIP,
                      .imin = 10,
                      .imax = 50 },   // steam version reorders this with the MainMenu::OnKeyDown call
                    { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argcap = { CT_MATCH0 } },
                   { DISASM_SKIP, .imin = 3, .imax = 10 },
                   { .inst = I_CALL, .argfilter = { ARG_MATCH }, .argsym = &SYM(operator_delete) },
                   { DISASM_SKIP, .imin = 3, .imax = 10 },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ESP } } },
                   { .inst = I_MOV, .argfilter = { ARG_REG }, .args = { { .base = REG_ESP } } },
                   { .inst = I_CALL, .mark = MARK_ARG1 },
                   { DISASM_FINISH } }
};
Symbol SYM(CommandGui_KeyDown) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CommandGui_KeyDown_Disasm },
             //             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7KeyDownEib" },
              { 0 } }
};
FuncInfo FUNCINFO(CommandGui_KeyDown) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};
