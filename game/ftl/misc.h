#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

DECLSYM(ftl_log);
DECLSYM(operator_new);
DECLSYM(operator_delete);

DECLSYM(basicstring_from_int);

typedef int32_t (*FUNCTYPE(random32))();
DECLFUNC(random32);

// These aren't really addresses, but we use the symbol framework to extract them from the
// disassembly.
DECLSYM(version_major);
#define g_version_major ((int)symAddr(ftlbase, version_major))
DECLSYM(version_minor);
#define g_version_minor ((int)symAddr(ftlbase, version_minor))
DECLSYM(version_rev);
#define g_version_rev ((int)symAddr(ftlbase, version_rev))
