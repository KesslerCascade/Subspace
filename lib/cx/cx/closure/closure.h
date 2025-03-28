#pragma once

#include <cx/cx.h>
#include <cx/stype/stvar.h>
#include <cx/utils/macros/args.h>

// Closures are a simple mechanism for capturing a list of variants to use as a function's
// environment when called later.

typedef struct closure_ref
{
    void *_is_closure;
} closure_ref;

typedef struct closure_ref *closure;

typedef bool (*closureFunc)(stvlist *cvars, stvlist *args);

_Ret_valid_ closure _closureCreate(_In_ closureFunc func, int n, stvar cvars[]);
// closure closureCreate(closureFunc func, ...)
#define closureCreate(func, ...) _closureCreate(func, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

bool _closureCall(_In_ closure cls, int n, stvar args[]);
// bool closureCall(closure cls, ...)
#define closureCall(cls, ...) _closureCall(cls, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

_Ret_valid_ closure closureClone(_In_ closure cls);

void closureDestroy(_Inout_ptr_uninit_ closure *cls);
