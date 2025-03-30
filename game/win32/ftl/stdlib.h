#pragma once

#include <windows.h>
#include "ftl/ftl.h"
#include "hook/function.h"

// ftlbase is used here because we want the import trampoline address

typedef void * (*FUNCTYPE(msvcrt_malloc))(size_t size);
DECLFUNC(msvcrt_malloc);
#define malloc(sz) FCALL(ftlbase, msvcrt_malloc, sz);

typedef void (*FUNCTYPE(msvcrt_free))(void *ptr);
DECLFUNC(msvcrt_free);
#define free(p) FCALL(ftlbase, msvcrt_free, p);

typedef void* (*FUNCTYPE(msvcrt_malloc))(size_t size);
DECLFUNC(msvcrt_malloc);
#define malloc(sz) FCALL(ftlbase, msvcrt_malloc, sz);

typedef int (*FUNCTYPE(msvcrt_snprintf))(char* s, size_t n, const char* format, ...);
DECLFUNC(msvcrt_snprintf);
#define snprintf(s, n, format, ...) FCALL(ftlbase, msvcrt_snprintf, s, n, format, __VA_ARGS__);
