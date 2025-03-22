#pragma once

#include "codegen/codegen.h"
#include "symbol.h"

// symbol info macro
#define FUNC(name)     _func_##name
#define FUNCTYPE(name) _func_##name##_t
#define FUNCINFO(name) _funcinfo_##name
#define FUNCWRAP(name) _wrapper_##name

#define DECLWRAP(name) extern FUNCTYPE(name) FUNCWRAP(name)
#define INITWRAP(name) FUNCTYPE(name) FUNCWRAP(name) = 0

#define DECLINFO(name) extern FuncInfo FUNCINFO(name)

#define DECLFUNC(name) \
    DECLSYM(name);     \
    DECLINFO(name);    \
    DECLWRAP(name)

#define FCALL(base, name, ...)                                                                 \
    (FUNCWRAP(name) ? 0 : (FUNCWRAP(name) = wrapCreate(symAddr(base, name), &FUNCINFO(name))), \
     FUNCWRAP(name)(__VA_ARGS__))
