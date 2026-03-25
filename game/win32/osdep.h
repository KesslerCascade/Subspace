#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef _DEBUG
extern void osWriteDbg(const char* str);
#else
#define osWriteDbg(x)
#endif

void osShowError(const char* str);

bool osWriteFile(const char* fn, uint8_t* buf, size_t sz);

typedef int (*threadfunc_t)(void* data);
bool osStartThread(threadfunc_t func, void* data);

void osExit(int retcode);

// time format is OS-specific but can be up to 64 bits wide
int64_t osFrameTime();
void osNextFrame();

bool osAbsolutePathUTF8(const char* fname, char* buf, size_t bufsz);

bool osIsUsingDirect3D();
