#pragma once

#include <cx/closure/closure.h>
#include <cx/thread/atomic.h>

// Closure chains are (threadsafe) linked lists of closures. The primary intended use is to
// register one or more callbacks for events.

// They may have an optional token assigned which allows for the closure to be detached
// from the chain later.

// If provided, the token is passed to the closure function as the last closure variable,
// after any that were provided to cchainAttach.

typedef struct cchain_ref
{
    void *_is_closure_chain;
} cchain_ref;

_Success_(return) bool _cchainAttach(_Inout_ptr_opt_ cchain* chain, _In_ closureFunc func, intptr token, int n,
                   stvar cvars[]);
// bool cchainAttach(cchain *chain, closureFunc func, ...)
// Attaches a closure to the chain. Extra arguments will be available in the function's cvars list.
// Can fail if the closure has been consumed by a one-shot call.
#define cchainAttach(chain, func, ...) _cchainAttach(chain, func, 0, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })
// bool cchainAttachToken(cchain *chain, closureFunc func, intptr token, ...)
// Attaches a closure to the chain. Extra arguments will be available in the function's cvars list.
// The given token should be a nonzero value which can be used to detach only this specific instance
// using cchainDetach.
#define cchainAttachToken(chain, func, token, ...) _cchainAttach(chain, func, token, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

// Attempts to detach a previously attached closure from the chain. Token must match the
// token that was used when the closure was attached (0 if non was specified).
_Success_(return) bool cchainDetach(_Inout_ptr_opt_ cchain* chain, _In_ closureFunc func, intptr token);

_Success_(return) bool _cchainCall(_In_ptr_opt_ cchain* chain, int n, stvar args[]);
// bool cchainCall(cchain *chain, ...)
// Calls every closure in the chain with the given arguments.
// Returns true only if every function returns true.
// NOTE: Unlike closureCall, this requires a pointer to the chain in order
// to perform the atomic read for thread safety.
#define cchainCall(chain, ...) _cchainCall(chain, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

_Success_(return) bool _cchainCallOnce(_Inout_ptr_opt_ cchain* chain, int n, stvar args[]);
// bool cchainCallOnce(cchain *chain, ...)
// Calls every closure in the chain with the given arguments.
// Returns true only if every function returns true.
// Atomically invalidates the chain, rendering it ineligibaly for attachment.
// This can be used to avoid race conditions when adding closures to the chain.
#define cchainCallOnce(chain, ...) _cchainCallOnce(chain, count_macro_args(__VA_ARGS__), (stvar[]) { __VA_ARGS__ })

// Safely transfers a closure chain to a new target.
// Invalidates the source chain -- future attach attempts will fail!
_Success_(return) bool cchainTransfer(_Inout_ptr_opt_ cchain* dest, _Inout_ptr_opt_ cchain* src);

// Like transfer, but creates a clone
_Success_(return) bool cchainClone(_Inout_ptr_opt_ cchain* dest, _In_ptr_opt_ cchain* src);

// Resets a previously invalidated closure chain so that it can be reused.
_Success_(return) bool cchainReset(_Inout_ptr_opt_ cchain* chain);

// Removes all closures from the chain but leaves it in a usable state.
_Success_(return) bool cchainClear(_Inout_ptr_opt_ cchain* chain);

// Destroys the closure chain.
void cchainDestroy(_Inout_ptr_opt_ cchain* chain);
