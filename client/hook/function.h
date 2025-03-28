#pragma once

#include "codegen/codegen.h"
#include "symbol.h"

// symbol info macro
#define FUNC(name)      _func_##name
#define FUNCTYPE(name)  _func_##name##_t
#define FUNCINFO(name)  _funcinfo_##name
#define FUNCINFOP(name) _funcinfo_p_##name
#define FUNCWRAP(name)  _wrapper_##name

#define DECLWRAP(name) extern FUNCTYPE(name) FUNCWRAP(name)
#define INITWRAP(name) FUNCTYPE(name) FUNCWRAP(name) = 0

#define DECLINFO(name)  extern FuncInfo FUNCINFO(name)
#define DECLINFOP(name) extern FuncInfo* FUNCINFOP(name)

#define FUNCP_SELECT(pname, name)           \
    do {                                    \
        SYMP_SELECT(pname, name);           \
        FUNCINFOP(pname) = &FUNCINFO(name); \
    } while (0)

#define DECLFUNC(name) \
    DECLSYM(name);     \
    DECLINFO(name);    \
    DECLWRAP(name)

#define DECLFUNCP(name) \
    DECLSYMP(name);     \
    DECLINFOP(name);    \
    DECLWRAP(name)

#define FCALL(base, name, ...)                                                                 \
    (FUNCWRAP(name) ? 0 : (FUNCWRAP(name) = wrapCreate(symAddr(base, name), &FUNCINFO(name))), \
     FUNCWRAP(name)(__VA_ARGS__))

#define FCALLP(base, name, ...)                                                                 \
    (FUNCWRAP(name) ? 0 : (FUNCWRAP(name) = wrapCreate(symAddrP(base, name), FUNCINFOP(name))), \
     FUNCWRAP(name)(__VA_ARGS__))
