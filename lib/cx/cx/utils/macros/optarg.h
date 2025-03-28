#pragma once

#include "args.h"

// single optional argument
// filled with 0 if not present

#if !defined(_MSC_VER)
#define opt_arg(...) _opt_arg_dispatch_1(count_macro_args(dummy __VA_OPT__(,) __VA_ARGS__), (dummy __VA_OPT__(,) __VA_ARGS__))
#define _opt_arg_dispatch_1(n, args) _opt_arg_dispatch_2(n, args)
#define _opt_arg_dispatch_2(n, args) _opt_arg_##n args
#define _opt_arg_1(f1) 0
#define _opt_arg_2(f1, f2) f2

// legacy compatibility
#define opt_flags(...) _opt_arg_dispatch_1(count_macro_args(dummy __VA_OPT__(,) __VA_ARGS__), (dummy __VA_OPT__(,) __VA_ARGS__))
#else
// Hacky workaround for MSVC's broken preprocessor
#define opt_arg(...) _opt_arg_expand(_opt_arg_dispatch(_opt_arg_catch_empty##__VA_ARGS__, __VA_ARGS__))
#define _opt_arg_expand(x) x
#define _opt_arg_catch_empty dummy1,_opt_arg_catch_paren
#define _opt_arg_catch_paren(x) dummy3,dummy4
#define _opt_arg_dispatch2(_1, _2, _3, _4, MACRO, ...) MACRO
#define _opt_arg_dispatch(...) _opt_arg_expand(_opt_arg_dispatch2(__VA_ARGS__, _opt_arg_1a, _opt_arg_0, _opt_arg_1, ERROR)(__VA_ARGS__))
#define _opt_arg_0(...) 0
#define _opt_arg_1(dummy1, f1) f1
#define _opt_arg_1a(dummy1, dummy2, dummy3, f1) f1

// legacy compatibility
#define opt_flags(...) _opt_arg_expand(_opt_arg_dispatch(_opt_arg_catch_empty##__VA_ARGS__, __VA_ARGS__))
#endif
