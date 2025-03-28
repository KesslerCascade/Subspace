#pragma once

#if defined(__aarch64__)
#define _CPU_PAUSE __asm__ volatile("yield")
#elif defined(__EMSCRIPTEN__)
// WebAssembly doesn't have a spinloop hint
#define _CPU_PAUSE
#else
#define _CPU_PAUSE __asm__ volatile("pause")
#endif
