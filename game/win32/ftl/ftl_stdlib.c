#include "ftl/stdlib.h"

INITWRAP(msvcrt_malloc);
Symbol SYM(msvcrt_malloc) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "malloc" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_malloc) = { .nargs = 1, .purecdecl = true };

INITWRAP(msvcrt_free);
Symbol SYM(msvcrt_free) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "free" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_free) = { .nargs = 1, .purecdecl = true };

INITWRAP(msvcrt_snprintf);
Symbol SYM(msvcrt_snprintf) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "_snprintf" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_snprintf) = { .nargs = 3, .purecdecl = true };

INITWRAP(msvcrt_sin);
Symbol SYM(msvcrt_sin) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "sin" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_sin) = { .nargs = 1, .purecdecl = true };
