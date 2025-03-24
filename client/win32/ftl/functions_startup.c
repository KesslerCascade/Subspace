#include "functions_startup.h"

INITWRAP(WinMain);
Symbol SYM(WinMain) = {
    .find = { { .type = SYMBOL_FIND_EXPORT, .name = "WinMain@16" }, { 0 } }
};
FuncInfo FUNCINFO(WinMain) = {
    .nargs = 4,
    .stdcall = true,
    .args = {
        { 4, ARG_INT, 0, true },
        { 4, ARG_INT, 0, true },
        { 4, ARG_PTR, 0, true },
        { 4, ARG_INT, 0, true },
    }
};
