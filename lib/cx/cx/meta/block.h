#pragma once

// Block wrapping using complex for statements
// Inspired by P99 preprocessor macros, modified (in some cases heavily)
// to fit with CX design philosophy.
// https://gustedt.gitlabpages.inria.fr/p99/p99-html/

#include <cx/stype/stype.h>
#include <cx/platform/base.h>
#include <cx/utils/macros/tokens.h>

// Compile-time checks for inhibited features
// This are declared here rather than a separate include file because they are co-dependent
// on the block macros, which in turn requires these to check for incorrect usage of "return"

#define _inhibit_name(name) _inhibit_##name

// Declare a feature that can be inhibited at compile time
#define inhibitDeclare(name) enum { _inhibit_name(name) = 0 }

// Results in a compile error if the specific feature is inhibited in the current block
// This switch construct triggers a compiler bug on versions of the MSVC compiler prior to VS2022,
// so disable it on those versions.
#if !defined(_MSC_VER) || _MSC_VER > 1930
#define inhibitCheck(name) switch(tokstring(_inhibit_name(name))[_inhibit_name(name)]) default:
#else
#define inhibitCheck(name)
#endif

// The internal versions are for contexts when we know we're guaranteed to already have a var and not
// need another _blockStart
#define _inhibitDisallow(name) _blkCond(const int * const _inhibit_name(name) = 0, !_inhibit_name(name))
// Inhibits the use of a feature inside the given block
#define inhibitDisallow(name) _blkStart _inhibitDisallow(name)
//for (const int *_inhibit_name(name) = 0; !_inhibit_name(name) && _BLK_VAR; _BLK_VAR = 0)

#define _inhibitAllow(name) _blkCond(const int _inhibit_name(name) = 0, !_inhibit_name(name))
// Allows the use of a feature inside the given block
#define inhibitAllow(name) _blkStart _inhibitAllow(name)
// for (const int _inhibit_name(name) = 0; !_inhibit_name(name) && _BLK_VAR; _BLK_VAR = 0)

// The "RETURN" feature is used internally for correctness checks

// If this is being compiled in debug/dev mode, redefine "return" to check it for correctness
#if DEBUG_LEVEL >= 1 && !defined(_PREFAST_)
inhibitDeclare(RETURN);
#define _inhibitReturn _inhibitDisallow(RETURN)
#define _allowReturn _inhibitAllow(RETURN)
#define return inhibitCheck(RETURN) return
#else
#define _inhibitReturn _blkStart
#define _allowReturn _blkStart
#endif

// ---------- Block Macros ----------

#define _BLK_VAR _block_done

// Internal building blocks (no pun intended)

#define _blkDef(before) for (tokeval(before); _BLK_VAR; _BLK_VAR = 0)
#define _blkCond(before, cond) for (tokeval(before); (cond) && _BLK_VAR; _BLK_VAR = 0)
#define _blkFull(before, cond, ...) for (tokeval(before); (cond) && _BLK_VAR; (__VA_ARGS__), _BLK_VAR = 0)

// _blkStart should be the first token used when building a structure that uses blocks.
// It declares the marker variable that is used to ensure the various for loop abuse only executes once.
#define _blkStart _blkDef(bool _BLK_VAR = 1)
#define _blkBefore(...) for (tokeval(__VA_ARGS__); _BLK_VAR; _BLK_VAR = 0)
#define _blkBeforeAfter(before, ...) _inhibitReturn _blkFull(tokeval(before), true, __VA_ARGS__)
#define _blkAfter(...) _blkBeforeAfter(, (__VA_ARGS__))
// _blkEnd is used as an inner loop around a user-provided block to swallow 'break' so it
// doesn't interrupt control flow.
#define _blkEnd _blkBefore()

// Special helper for declaring a scoped variable that can refer to a variable in the outer scope with
// the same name. It does this by using a temporary variable with a different name.
#define _blkDefRecursive(type, name, ...)                                                                       \
    _blkDef(type tokcat2(_block_decl_, name) = tokeval(__VA_ARGS__))                                            \
    _blkCond(type name = tokcat2(_block_decl_, name), ((void)name, true))

// blkWrap(before, after) {}
// Wraps the given block with "before" and "after" statements. Early exit from the block is possible
// with a break or a continue statement at the base level; but the "after" statement is executed
// even if the block is exited early.
// This version does not support fancy unwinding but is quite lightweight and suitable for use in
// most situations.
#define blkWrap(before, ...) _blkStart _blkBeforeAfter(tokeval(before), __VA_ARGS__) _blkEnd
