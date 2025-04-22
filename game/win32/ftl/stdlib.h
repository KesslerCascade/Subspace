#pragma once

#include <windows.h>
#include "ftl/ftl.h"
#include "hook/function.h"

// ftlbase is used here because we want the import trampoline address

typedef void * (*FUNCTYPE(msvcrt_malloc))(size_t size);
DECLFUNC(msvcrt_malloc);
#define malloc(sz) FCALL(ftlbase, msvcrt_malloc, sz)

typedef void (*FUNCTYPE(msvcrt_free))(void *ptr);
DECLFUNC(msvcrt_free);
#define free(p) FCALL(ftlbase, msvcrt_free, p)

typedef void* (*FUNCTYPE(msvcrt_malloc))(size_t size);
DECLFUNC(msvcrt_malloc);
#define malloc(sz) FCALL(ftlbase, msvcrt_malloc, sz)

typedef void* (*FUNCTYPE(msvcrt_realloc))(void* ptr, size_t size);
DECLFUNC(msvcrt_realloc);
#define realloc(ptr, sz) FCALL(ftlbase, msvcrt_realloc, ptr, sz)

typedef int (*FUNCTYPE(msvcrt_snprintf))(char* s, size_t n, const char* format, ...);
DECLFUNC(msvcrt_snprintf);
#define snprintf(s, n, format, ...) FCALL(ftlbase, msvcrt_snprintf, s, n, format, __VA_ARGS__)

typedef double (*FUNCTYPE(msvcrt_sin))(double x);
DECLFUNC(msvcrt_sin);
#define sin(x) FCALL(ftlbase, msvcrt_sin, x)

typedef uintptr_t (*FUNCTYPE(msvcrt_beginthread))(void (*start_address)(void*), unsigned stack_size,
                                                  void* arglist);
DECLFUNC(msvcrt_beginthread);
#define _beginthread(start_address, stack_size, arglist) \
    FCALL(ftlbase, msvcrt_beginthread, start_address, stack_size, arglist)

typedef int (*FUNCTYPE(msvcrt_rand))(void);
DECLFUNC(msvcrt_rand);
#define rand() FCALL(ftlbase, msvcrt_rand)

typedef void (*FUNCTYPE(msvcrt_srand))(unsigned seed);
DECLFUNC(msvcrt_srand);
#define srand(seed) FCALL(ftlbase, msvcrt_srand, seed)

typedef time_t (*FUNCTYPE(msvcrt_time))(time_t* tm);
DECLFUNC(msvcrt_time);
#define time(tm) FCALL(ftlbase, msvcrt_time, tm)
