#pragma once

#include <cx/meta/block.h>
#include <cx/utils/macros/unused.h>
#include <setjmp.h>

enum _pblock_unwind_const {
    _pblock_unwind_top = 0
};

typedef struct _pblock_jmp_buf_node _pblock_jmp_buf_node;
typedef struct _pblock_jmp_buf_node {
    _pblock_jmp_buf_node *next;                 // used for link list in try/catch code
    jmp_buf buf;                                // longjmp destination to exit the current block
    volatile int target;                        // how many levels of nesting are we unwinding
} _pblock_jmp_buf_node;

typedef _pblock_jmp_buf_node _pblock_jmp_buf[1];
#define BLOCK_JMPBUF_INIT {[0] = {.target = 0}}

_meta_inline /*noreturn*/ void _pbLongjmp(_pblock_jmp_buf_node *node, int val)
{
    node->target = val;
    longjmp(node->buf, 1);
}

// pblock {}
// Declares a protected block. It sets up a setjmp buffer for stack unwinding that can be jumped
// directly to using pblockUnwind. Protected blocks can also be nested and an arbitrary number of
// layers can be broken out of.
//
// A protected block can optionally end in a section with a PBLOCK_AFTER pseudo-label. The
// instructions after this label are executed when exiting the block normally, as well as when
// the block is exited early via unwinding.
//
// This is the basic building block on which try/catch is implemented, but can be used separately
// from it.
//
// Note that protected blocks are quite heavyweight. They add a measurable amount of execution time
// both from the setjmp setup as well as the fact that the use of setjmp/longjmp inhibits numerous
// compiler optimizations from being performed on the block. Additionally, each nested block consumes
// a fairly large amount of stack space. They should be used only around complex operations that are
// not performance critical.
#define pblock                                                                                                  \
    _blkStart                                                                                                   \
    _inhibitReturn                                                                                              \
    /* set to true during the unwinding process */                                                              \
    _blkDef(volatile bool _pblock_unwind = false)                                                               \
    _blkFull(/* BEFORE */ volatile int _pblock_target = 0, true,                                                \
        /* After we exit the block through whatever means, if we are unwinding, try to unwind the next level.   \
         * Since this loop is above the one that defines _pblock_unwind_top, the jump buffer here points to the \
         * one set by the block that encloses this one and we'll jump back into it, giving it the ability to    \
         * run any PBLOCK_AFTER cleanup and continue the chain.                                                 \
         * In the outermost block, _pblock_unwind_top will instead be 0 from the global enum, and pblockUnwind  \
         * will do nothing.                                                                                     \
         * If _pblock_target reaches 0, this will also exit the loop and continue execution as normal. */       \
        /* AFTER */ (_pblock_unwind ?                                                                           \
            pblockUnwind(_pblock_target <= 0 ? _pblock_target : _pblock_target - 1) : nop_stmt))                \
    /* allocate a jump buffer on the stack for this level of recursion */                                       \
    _blkDef(_pblock_jmp_buf _pblock_unwind_top = tokeval(BLOCK_JMPBUF_INIT))                                    \
    /* set the longjump target to be within the inntermost loop and coerce the return value to 1 or 0 */        \
    switch(setjmp(_pblock_unwind_top[0].buf))                                                                   \
        /* case 2 doesn't actually exist, it serves as a place to hang an if-else construct so we can define    \
           two case labels here instead of only one - case 0 for the user block to execute, and a default       \
           case label that acts as a fallback for case 1 not being defined, which can happen if the user does   \
           not include a PBLOCK_AFTER label. */                                                                 \
        case 2: if (0) {                                                                                        \
            default:                                                                                            \
                /* if there isn't a PBLOCK_AFTER but we got here as a longjmp target, we need to take care of   \
                 * setting up the variable for unwinding, otherwise the AFTER blocks will fail to jump to the   \
                 * next target in the chain and we'll stop unwinding early. */                                  \
                _pblock_target = _pblock_unwind_top[0].target;                                                  \
                _pblock_unwind = !!_pblock_target;                                                              \
        } else                                                                                                  \
        /* This is the case for when we get here the first time, right after calling setjmp. The user-provided  \
         * block slots in directly after this label. */                                                         \
        case 0:


_meta_inline void _pblockUnwind(void *top, int cond)
{
    if (cond && top) _pbLongjmp(top, cond);
}

// pblockUnwind(num)
// Exits the current protected block early. num may be the number of nested blocks to break out of,
// starting at 1 for the current block only. It may also be -1 which breaks out of any number of protected
// blocks and returns to the instruction immediately after the outermost layer.
#define pblockUnwind(num) _pblockUnwind((void*)_pblock_unwind_top, (num))

// PBLOCK_AFTER:
// This pseudo-label is similar to a finally clause in exception handling. It is guaranteed to be
// executed before control flow leaves the protected block that it is contained within, regardless
// of how that occurs (except for abnormal control flow like longjmp).
#define PBLOCK_AFTER                                                                                            \
    if (0) {                                                                                                    \
        /* Even though this case 1 label is placed into the user-defined block, it technically exists within    \
         * the switch statement at the end of the pblock macro.                                                 \
         * When the user provides a PBLOCK_AFTER target, it defines this case 1 which overrides the default     \
         * case label (the one that's hidden in the if(0) after the case 2:) and comes here instead. */         \
        case 1:                                                                                                 \
            _pblock_target = _pblock_unwind_top[0].target;                                                      \
            _pblock_unwind = !!_pblock_target;                                                                  \
            /* Execution falls through to the statement below. */                                               \
    }                                                                                                           \
    /* This odd looking construct exists to swallow the ':' after the fake PBLOCK_AFTER label. */               \
    switch(0) case 0
