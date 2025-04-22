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

INITWRAP(msvcrt_realloc);
Symbol SYM(msvcrt_realloc) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "realloc" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_realloc) = { .nargs = 2, .purecdecl = true };

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

INITWRAP(msvcrt_beginthread);
Symbol SYM(msvcrt_beginthread) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "_beginthread" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_beginthread) = { .nargs = 3, .purecdecl = true };

INITWRAP(msvcrt_rand);
Symbol SYM(msvcrt_rand) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "rand" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_rand) = { .nargs = 0, .purecdecl = true };

INITWRAP(msvcrt_srand);
Symbol SYM(msvcrt_srand) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "srand" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_srand) = { .nargs = 1, .purecdecl = true };

INITWRAP(msvcrt_time);
Symbol SYM(msvcrt_time) = {
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "time" }, { 0 } }
};
FuncInfo FUNCINFO(msvcrt_time) = { .nargs = 1, .purecdecl = true };