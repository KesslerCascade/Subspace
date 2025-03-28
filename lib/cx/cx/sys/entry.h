#pragma once

// Platform-independent entry point

#include <cx/cx.h>
#include <cx/platform/base.h>
#include <cx/container/sarray.h>

// Command-line arguments (UTF-8)
extern string cmdProgram;
extern sa_string cmdArgs;

// DEFINE_ENTRY_POINT
// Put this macro in a translation unit somewhere to define the platform
// dependent entry point, which will parse the command-line arguments and
// call entryPoint()

// User-supplied function that is called by DEFINE_ENTRY_POINT
int entryPoint();

// Internal support functions, do not call these directly
void _entryParseArgs(int argc, const char **argv);
void _entryParseArgsU16(int argc, const uint16 **argv);

#if defined(_PLATFORM_WIN)
#include "cx/platform/win/win_sys_entry.h"
#elif defined(_PLATFORM_UNIX)
#include "cx/platform/unix/unix_sys_entry.h"
#elif defined (_PLATFORM_WASM)
#include "cx/platform/wasm/wasm_sys_entry.h"
#endif
