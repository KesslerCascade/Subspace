#pragma once

#if !defined(__clang__) && defined(__GNUC__)
#include <stdint.h>
__attribute__((always_inline)) inline int _unused_helper(intptr_t _dummy) { (void)_dummy; return 0; }
#define unused_noeval(x) (_unused_helper(true ? (intptr_t)0 : (intptr_t)(x)))
#else
#define unused_noeval(x) (true ? (void)0 : (void)(x))
#endif
#define nop_stmt ((void)0)
