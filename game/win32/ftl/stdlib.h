#pragma once

#include <windows.h>
#include "ftl/ftl.h"
#include "hook/function.h"

// ftlbase is used here because we want the import trampoline address

typedef void* (*FUNCTYPE(crt_malloc))(size_t size);
DECLFUNC(crt_malloc);
#define malloc(sz) FCALL(ftlbase, crt_malloc, sz)

typedef void (*FUNCTYPE(crt_free))(void* ptr);
DECLFUNC(crt_free);
#define free(p) FCALL(ftlbase, crt_free, p)

typedef void* (*FUNCTYPE(crt_malloc))(size_t size);
DECLFUNC(crt_malloc);
#define malloc(sz) FCALL(ftlbase, crt_malloc, sz)

typedef void* (*FUNCTYPE(crt_realloc))(void* ptr, size_t size);
DECLFUNC(crt_realloc);
#define realloc(ptr, sz) FCALL(ftlbase, crt_realloc, ptr, sz)

typedef int (*FUNCTYPE(crt_snprintf))(char* s, size_t n, const char* format, ...);
DECLFUNC(crt_snprintf);
#define snprintf(s, n, format, ...) FCALL(ftlbase, crt_snprintf, s, n, format, __VA_ARGS__)

typedef double (*FUNCTYPE(crt_sin))(double x);
DECLFUNC(crt_sin);
#define sin(x) FCALL(ftlbase, crt_sin, x)

typedef uintptr_t (*FUNCTYPE(crt_beginthread))(void (*start_address)(void*), unsigned stack_size,
                                               void* arglist);
DECLFUNC(crt_beginthread);
#define _beginthread(start_address, stack_size, arglist) \
    FCALL(ftlbase, crt_beginthread, start_address, stack_size, arglist)

typedef int (*FUNCTYPE(crt_rand))(void);
DECLFUNC(crt_rand);
#define rand() FCALL(ftlbase, crt_rand)

typedef void (*FUNCTYPE(crt_srand))(unsigned seed);
DECLFUNC(crt_srand);
#define srand(seed) FCALL(ftlbase, crt_srand, seed)

typedef time_t (*FUNCTYPE(crt_time))(time_t* tm);
DECLFUNC(crt_time);
#define time(tm) FCALL(ftlbase, crt_time, tm)
