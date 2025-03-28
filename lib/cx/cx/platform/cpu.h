#pragma once

#include <cx/platform/base.h>

#if defined(_COMPILER_MSVC)
#include <cx/platform/msvc/msvc_cpu.h>
#elif defined(_COMPILER_CLANG) || defined(_COMPILER_GCC)
#include <cx/platform/clang/clang_cpu.h>
#endif
