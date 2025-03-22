#pragma once
#include "codegen.h"

// Creates a hook function.
//
// If pre is given (non-NULL), the hook calls pre() with the same arguments
// as it was called with. If pre() returns zero, the hook function immediately
// returns zero as well.
//
// If pre is not given or if the pre() function returns nonzero, the
// original function at addr is called with its original arguments.
//
// Finally, if post is non-NULL, the post() function is called after the
// original returns with the original arguments prepended with an
// extra first argument, which is the return of addr(). The return of
// post() is used as the return of the hook function.
void* hookCreate(addr_t addr, const FuncInfo* fi, void* pre, void* post);

// Creates a replacement function, or 'inverse wrapper'. If the function
// identified by addr and fi uses a custom calling convention, the wrapper
// adapts the parameters so that replacement() can be called instead
// using the standard cdecl convention.
//
// Intended for functions that completely replace the original, or need
// more flexibility and call the original themselves using one of the
// standard wrappers (see wrapgen.h and functions.h).
void* replacementCreate(const FuncInfo* fi, void* replacement);
