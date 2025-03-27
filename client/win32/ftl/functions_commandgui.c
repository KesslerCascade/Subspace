#include "ftl/functions_commandgui.h"
#include "ftl/functions_capp.h"
#include "ftl/functions_misc.h"

INITWRAP(CommandGui_KeyDown);
Symbol SYM(CommandGui_KeyDown) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_OnKeyDown_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui7KeyDownEib" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_KeyDown) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};

INITWRAP(CommandGui_SetPaused);
Symbol SYM(CommandGui_SetPaused) = {
    .find = { { .type = SYMBOL_FIND_DISASM, .disasm = &CApp_GenInputEvents_trace },
             { .type = SYMBOL_FIND_EXPORT, .name = "_ZN10CommandGui9SetPausedEbb" },
             { 0 } }
};
FuncInfo FUNCINFO(CommandGui_SetPaused) = {
    .nargs   = 3,
    .stdcall = true,
    .args    = { { 4, ARG_PTR, REG_ECX, false }, { 4, ARG_INT, 0, true }, { 4, ARG_INT, 0, true } }
};
