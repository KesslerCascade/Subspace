#include "ftl/stdlib.h"

INITWRAP(crt_malloc);
Symbol SYM(crt_malloc) = {
    SYMNAME("malloc"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "malloc" }, { 0 } }
};
FuncInfo FUNCINFO(crt_malloc) = { .nargs = 1, .purecdecl = true };

INITWRAP(crt_free);
Symbol SYM(crt_free) = {
    SYMNAME("free"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "free" }, { 0 } }
};
FuncInfo FUNCINFO(crt_free) = { .nargs = 1, .purecdecl = true };

INITWRAP(crt_realloc);
Symbol SYM(crt_realloc) = {
    SYMNAME("realloc"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "realloc" }, { 0 } }
};
FuncInfo FUNCINFO(crt_realloc) = { .nargs = 2, .purecdecl = true };

INITWRAP(crt_snprintf);
Symbol SYM(crt_snprintf) = {
    SYMNAME("snprintf"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "_snprintf" }, { 0 } }
};
FuncInfo FUNCINFO(crt_snprintf) = { .nargs = 3, .purecdecl = true };

INITWRAP(crt_sin);
Symbol SYM(crt_sin) = {
    SYMNAME("sin"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "sin" }, { 0 } }
};
FuncInfo FUNCINFO(crt_sin) = { .nargs = 1, .purecdecl = true };

INITWRAP(crt_beginthread);
Symbol SYM(crt_beginthread) = {
    SYMNAME("beginthread"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "_beginthread" }, { 0 } }
};
FuncInfo FUNCINFO(crt_beginthread) = { .nargs = 3, .purecdecl = true };

INITWRAP(crt_rand);
Symbol SYM(crt_rand) = {
    SYMNAME("rand"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "rand" }, { 0 } }
};
FuncInfo FUNCINFO(crt_rand) = { .nargs = 0, .purecdecl = true };

INITWRAP(crt_srand);
Symbol SYM(crt_srand) = {
    SYMNAME("srand"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "srand" }, { 0 } }
};
FuncInfo FUNCINFO(crt_srand) = { .nargs = 1, .purecdecl = true };

INITWRAP(crt_time);
Symbol SYM(crt_time) = {
    SYMNAME("time"),
    .find = { { .type = SYMBOL_FIND_IMPORT, .str = "msvcrt.dll", .name = "time" }, { 0 } }
};
FuncInfo FUNCINFO(crt_time) = { .nargs = 1, .purecdecl = true };