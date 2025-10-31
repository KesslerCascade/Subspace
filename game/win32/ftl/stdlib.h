#pragma once

#include <cx/cx.h>
#include <windows.h>
#include "ftl/ftl.h"
#include "hook/function.h"

// ftlbase is used here because we want the import trampoline address

typedef void* (*FUNCTYPE(crt_malloc))(size_t size);
DECLFUNC(crt_malloc);
#define ftl_malloc(sz) FCALL(ftlbase, crt_malloc, sz)

typedef void (*FUNCTYPE(crt_free))(void* ptr);
DECLFUNC(crt_free);
#define ftl_free(p) FCALL(ftlbase, crt_free, p)

typedef void* (*FUNCTYPE(crt_realloc))(void* ptr, size_t size);
DECLFUNC(crt_realloc);
#define ftl_realloc(ptr, sz) FCALL(ftlbase, crt_realloc, ptr, sz)

typedef char* (*FUNCTYPE(crt_strdup))(const char* src);
DECLFUNC(crt_strdup);
#define ftl_strdup(src) FCALL(ftlbase, crt_strdup, src)

typedef int (*FUNCTYPE(crt_snprintf))(char* s, size_t n, const char* format, ...);
DECLFUNC(crt_snprintf);
#define ftl_snprintf(s, n, format, ...) FCALL(ftlbase, crt_snprintf, s, n, format, __VA_ARGS__)

typedef double (*FUNCTYPE(crt_sin))(double x);
DECLFUNC(crt_sin);
#define ftl_sin(x) FCALL(ftlbase, crt_sin, x)

typedef uintptr_t (*FUNCTYPE(crt_beginthread))(void (*start_address)(void*), unsigned stack_size,
                                               void* arglist);
DECLFUNC(crt_beginthread);
#define ftl_beginthread(start_address, stack_size, arglist) \
    FCALL(ftlbase, crt_beginthread, start_address, stack_size, arglist)

typedef int (*FUNCTYPE(crt_rand))(void);
DECLFUNC(crt_rand);
#define ftl_rand() FCALL(ftlbase, crt_rand)

typedef void (*FUNCTYPE(crt_srand))(unsigned seed);
DECLFUNC(crt_srand);
#define ftl_srand(seed) FCALL(ftlbase, crt_srand, seed)

typedef time_t (*FUNCTYPE(crt_time))(time_t* tm);
DECLFUNC(crt_time);
#define ftl_time(tm) FCALL(ftlbase, crt_time, tm)
