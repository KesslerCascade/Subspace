#pragma once

#include <cx/meta/pblock.h>

// Builds on pblock to implement try/catch/finally blocks.
// Note that this implementation has important semantic differences between some other languages and their
// meaning of catch and finally, see the documentation below for details!

// IMPORTANT NOTE: Though this is built on pblocks, you CANNOT mix and match pblocks and exceptions in the
// same call graph. If exceptions are in use, only ptTry/ptCatch/ptFinally should be used. In particular,
// PBLOCK_AFTER labels will be skipped over while exceptions are being processed, and ptThrow inside of a
// non-ptTry pblock will cause improper execution flow.

typedef struct ExceptionInfo {
    int code;                   // exception code, can be an application-specific constant
    const char *msg;            // exception message, MUST be a static constant, not something on the stack
#if DEBUG_LEVEL >= 1
    const char *file;           // filename
    int ln;                     // line number
#endif
} ExceptionInfo;

extern _Thread_local _pblock_jmp_buf_node *_ptry_top;           // stack of jump buffers to rewind to
extern _Thread_local ExceptionInfo _ptry_exc;                   // currently active exception
extern ExceptionInfo _ptry_exc_empty;

typedef int (*ptUnhandledHandler)(ExceptionInfo *einfo);

// GCC falsely warns that _ptry_top is a dangling pointer, even though we're very careful to clean
// it up when exiting the scope that the local variable it points to is declared in.
#if defined(__GNUC__) && __GNUC__ >= 12
#define ptDisablePtrWarning _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdangling-pointer\"")
#define ptReenableWarnings _Pragma("GCC diagnostic pop")
#else
#define ptDisablePtrWarning
#define ptReenableWarnings
#endif

// Registers a handler-of-last-resort for any uncaught exceptions.
// This handler may return 0 to abort (crash) the program, or it may return 1 to resume execution as if the
// exception had been caught. Execution will resume after the outermost ptFinally block, or, in the event
// that there are no try blocks at all, immediately after the exception is thrown.
//
// If multiple handlers are registered, they must ALL return 1 in order for execution to resume.
void ptRegisterUnhandled(ptUnhandledHandler handler);

// Unregisters a previously registered handler for uncaught exceptions.
void ptUnregisterUnhandled(ptUnhandledHandler handler);

void _ptry_handle_unhandled(ExceptionInfo *einfo);

_meta_inline void _ptry_clear(void)
{
    _ptry_exc = _ptry_exc_empty;
}

_meta_inline void _ptry_push(_pblock_jmp_buf_node *node)
{
    node->next = _ptry_top;
    ptDisablePtrWarning;
    _ptry_top = node;
    ptReenableWarnings;
}

_meta_inline _pblock_jmp_buf_node *_ptry_pop(void)
{
    _pblock_jmp_buf_node *ret = _ptry_top;
    if (!ret)
        return ret;

    _ptry_top = ret->next;
    ret->next = NULL;
    return ret;
}

_meta_inline void _ptry_pop_until(_pblock_jmp_buf_node *node)
{
    _pblock_jmp_buf_node *last;
    // keep popping until we hit the specified node or run out of jump buffers
    do {
        last = _ptry_pop();
    } while (last && last != node);
}

#if DEBUG_LEVEL >= 1
_meta_inline void _ptry_throw(int code, const char *msg, const char *file, int ln)
#else
_meta_inline void _ptry_throw(int code, const char *msg)
#endif
{
    // populate the thread-local exception info as we go into exception-handling mode
    _ptry_exc.code = code;
    _ptry_exc.msg = msg;
#if DEBUG_LEVEL >= 1
    _ptry_exc.file = file;
    _ptry_exc.ln = ln;
#endif

    // jump to the handler block if we have one, in most cases this function stops executing here
    if (_ptry_top) _pbLongjmp(_ptry_top, -1);

    // otherwise call the unhandled exception handler, this will probably abort and never return
    _ptry_handle_unhandled(&_ptry_exc);

    // if it DOES return, terminate exception processing
    _ptry_clear();
}

// ptThrow(int code, const char *msg)
// Throws an exception to be caught by a ptCatch block somewhere up the stack.
// There are two parts to an exception, the numeric code, which is an application-specific constant, and a message.
// The message MUST be a static string literal and can NOT be heap or stack allocated.
#if DEBUG_LEVEL >= 1
#define ptThrow(code, msg) _ptry_throw(code, msg, __FILE__, __LINE__)
#else
#define ptThrow(code, msg) _ptry_throw(code, msg)
#endif

// ptRethrow
// For use inside of a ptCatch block, ptRethrow is used to re-throw the active exception to a handler further
// up the call graph. This is most commonly used when filtering specific exceptions by code to be handled
// locally.
#if DEBUG_LEVEL >= 1
#define ptRethrow _ptry_throw(_ptry_exc.code, _ptry_exc.msg, _ptry_exc.file, _ptry_exc.ln)
#else
#define ptRethrow _ptry_throw(_ptry_exc.code, _ptry_exc.msg)
#endif

// ptTry { }
// If an exception occurs inside of a ptTry block, execution is transferred to the immediately following ptCatch
// or ptFinally block. Only one may be used, and one or the other is REQUIRED. The difference between the two
// is that ptFinally results in the exception propagating up to the caller's ptCatch/ptFinally, while ptCatch
// stops the exception locally and continues execution normally, unless ptRethrow is used.
//
// If the exception is not caught, the unhandled exception handler is invoked. The default handler terminates the
// program with an error.
#define ptTry                                                                                       \
    pblock                                                                                          \
    /* phase 0 is the try, phase 1 is the catch/finally */                                          \
    for (unsigned _ptry_phase = 0; _ptry_phase < 2; ++_ptry_phase)                                  \
        /* try phase */                                                                             \
        if (!_ptry_phase)                                                                           \
            /* push the current pblock jump target to the exception handler stack */                \
            /* this will cause it to be entered through the switch label */                         \
            _blkBefore(_ptry_push(_pblock_unwind_top))                                              \
            do

// common setup for catch/finally
#define _ptry_after_block                                                                           \
    /* this can be reached two ways, either naturally in phase 1 of the _ptry_phase loop (this is   \
     * the 'else'), or if an exception occurs, through the case 1: label through the pblock after   \
     * longjmping into the pblock loop from a lower stack frame. The latter is the same mechanism   \
     * that PBLOCK_AFTER would use, but for try/catch it's unconditional. */                        \
    while(0); else case 1:                                                                          \
    /* critically important to protect the outer block loop variable here! */                       \
    _blkStart                                                                                       \
    /* make sure that this phase isn't executed again (if we got here via longjmp) */               \
    _blkBefore(_ptry_phase = 2)                                                                     \
    /* unwind the handler stack to one level above where we currently are */                        \
    _blkBefore(_ptry_pop_until(_pblock_unwind_top))

// ptFinally {}
// The ptFinally block must immediately follow a ptTry. It is executed after the ptTry block either
// completes, or an exception occurs. In the case of an exception, after the ptFinally block is
// finished, exception handling continues up to the caller's ptFinally/ptCatch if one exists.
//
// ptFinally should be used to guarantee that cleanup is performed before the current stack frame
// unwinds in the case of an exception.
#define ptFinally                                                                                   \
    _ptry_after_block                                                                               \
    /* finally blocks end in an implicit rethrow if there is an active exception */                 \
    _blkAfter(_ptry_exc.code ? ptRethrow : nop_stmt)

// ptCatch {}
// A ptCatch block can immediately follow a ptTry. It is similar to ptFinally in that it allows a
// block of code to always be guaranteed to run after the ptTry is exited, but ptCatch stops further
// exception processing.
//
// There are a few key differences between ptCatch and try/catch in languages like C++.
//
// 1. ptCatch blocks are ALWAYS entered, just like ptFinally blocks. They are executed even if no
//    exception occurred. In this case, ptCode will be 0.
// 2. There is no built-in exception filtering. ptCatch will catch ALL exceptions. It is up to the
//    user to check ptCode for a specific code they're interested in, and use ptRethrow to rethrow
//    it if it's one that should be handled at a higher level.
#define ptCatch                                                                                     \
    _ptry_after_block                                                                               \
    _blkAfter(_ptry_clear())

// ptCode resolves to the code of the exception that is currently being processed, or 0 if there is none
#define ptCode _ptry_exc.code

// ptMsg resolves to the message of the exception that is currently being processed, or NULL if there is none
#define ptMsg _ptry_exc.msg
