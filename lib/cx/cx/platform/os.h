#pragma once

#include <cx/cx.h>
#include <cx/platform/base.h>

#if defined(_PLATFORM_WIN)
#include <cx/platform/win/win_os.h>
#elif defined(_PLATFORM_UNIX)
#include <cx/platform/unix/unix_os.h>
#elif defined(_PLATFORM_WASM)
#include <cx/platform/wasm/wasm_os.h>
#endif

void osYield();
void osSleep(int64 time);
int osPhysicalCPUs();
int osLogicalCPUs();
