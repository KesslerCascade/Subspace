#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef _DEBUG
extern void osWriteDbg(const char* str);
#else
#define osWriteDbg(x)
#endif

void osShowError(const char* str);

typedef int (*threadfunc_t)(void* data);
bool osStartThread(threadfunc_t func, void* data);
