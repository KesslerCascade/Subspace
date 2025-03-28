#include "pblock.h"
#include "ptry.h"
#include <cx/debug/assert.h>
#include <cx/thread/rwlock.h>
#include <cx/container/sarray.h>
#include <cx/container/foreach.h>
#include <cx/utils/lazyinit.h>

// define a few globals used by the macro library

// Top of the exception stack (LIFO)
_Thread_local _pblock_jmp_buf_node *_ptry_top;
_Thread_local ExceptionInfo _ptry_exc;
ExceptionInfo _ptry_exc_empty = { 0 };

saDeclare(ptUnhandledHandler);
static sa_ptUnhandledHandler handler_list;
static RWLock handler_lock;

static LazyInitState handler_init_state;
static void handler_init(void *dummy)
{
    saInit(&handler_list, ptr, 4);
    rwlockInit(&handler_lock);
}

void ptRegisterUnhandled(ptUnhandledHandler handler)
{
    lazyInit(&handler_init_state, handler_init, NULL);

    rwlockAcquireWrite(&handler_lock);
    saPush(&handler_list, ptr, handler, SA_Unique);
    rwlockReleaseWrite(&handler_lock);
}

void ptUnregisterUnhandled(ptUnhandledHandler handler)
{
    lazyInit(&handler_init_state, handler_init, NULL);
    rwlockAcquireWrite(&handler_lock);
    saFindRemove(&handler_list, ptr, handler);
    rwlockReleaseWrite(&handler_lock);
}

void _ptry_handle_unhandled(ExceptionInfo *einfo)
{
    lazyInit(&handler_init_state, handler_init, NULL);
    rwlockAcquireRead(&handler_lock);

    // try to resume execution if we have at least one handler that doesn't return 0
    bool resume = (saSize(handler_list) > 0);

    foreach(sarray, idx, ptUnhandledHandler, handler, handler_list) {
        if (handler(einfo) == 0)
            resume = false;
    }

    rwlockReleaseRead(&handler_lock);

    if (!resume)
#if DEBUG_LEVEL >= 1
        _cxAssertFail("Unhandled Exception", einfo->msg, einfo->file, einfo->ln);
#else
        _cxAssertFail("Unhandled Exception", einfo->msg);
#endif
}
