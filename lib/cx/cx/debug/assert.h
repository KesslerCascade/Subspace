#pragma once

#include <stdbool.h>
#include <cx/platform/base.h>
#include <cx/platform/cpp.h>
#include <cx/utils/macros/salieri.h>
#include <cx/utils/macros/unused.h>

// Callback function on assertion failure

enum ASSERT_ACTION_ENUM {
    ASSERT_Crash = 0,               // Intentionally crash in order to invoke a crash handler / reporter or get a memory dump
    ASSERT_Exit,                    // Immediately terminate the process    
    ASSERT_Ignore                   // Ignore the failure and try to continue
};

// Should return one of the ASSERT_ACTION_ENUM constants above.
// expr and/or msg may be NULL depending on if a message was given, or FatalError function called.
// file will be NULL in release builds (DEBUG_LEVEL == 0).
typedef int(*dbgAssertCallback)(_In_opt_z_ const char *expr, _In_opt_z_ const char *msg, _In_opt_z_ const char *file, int ln);

// Install function to call upon assertion failure
void dbgAssertAddCallback(dbgAssertCallback cb);
void dbgAssertRemoveCallback(dbgAssertCallback cb);

#if DEBUG_LEVEL >= 1
CX_C _no_inline bool _cxAssertFail(_In_opt_z_ const char *expr, _In_opt_z_ const char *msg, _In_z_ const char *file, int ln);
#else
CX_C _no_inline bool _cxAssertFail(_In_opt_z_ const char *expr, _In_opt_z_ const char *msg);
#endif

#if DEBUG_LEVEL >= 2
#define dbgAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL, __FILE__, __LINE__))
#define dbgAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg, __FILE__, __LINE__))
#define dbgVerify(expr) (!!(expr) || _cxAssertFail(#expr, NULL, __FILE__, __LINE__))
#define dbgVerifyMsg(expr, msg) (!!(expr) || _cxAssertFail(#expr, msg, __FILE__, __LINE__))
#else
#define dbgAssert(expr) unused_noeval(expr)
#define dbgAssertMsg(expr, msg) unused_noeval(expr)
#define dbgVerify(expr) (!!(expr))
#define dbgVerifyMsg(expr, msg) (!!(expr))
#endif

#if DEBUG_LEVEL >= 1
#define devAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL, __FILE__, __LINE__))
#define devAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg, __FILE__, __LINE__))
#define devVerify(expr) (!!(expr) || _cxAssertFail(#expr, NULL, __FILE__, __LINE__))
#define devVerifyMsg(expr, msg) (!!(expr) || _cxAssertFail(#expr, msg, __FILE__, __LINE__))
#define relAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL, __FILE__, __LINE__))
#define relAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg, __FILE__, __LINE__))
#define devFatalError(msg) _cxAssertFail(NULL, msg, __FILE__, __LINE__)
#define relFatalError(msg) _cxAssertFail(NULL, msg, __FILE__, __LINE__)
#elif defined(DIAGNOSTIC)
#define devAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL))
#define devAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg))
#define devVerify(expr) (!!(expr) || _cxAssertFail(#expr, NULL))
#define devVerifyMsg(expr, msg) (!!(expr) || _cxAssertFail(#expr, msg))
#define relAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL))
#define relAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg))
#define devFatalError(msg) _cxAssertFail(NULL, msg)
#define relFatalError(msg) _cxAssertFail(NULL, msg)
#else
#define devAssert(expr) unused_noeval(expr)
#define devAssertMsg(expr, msg) unused_noeval(expr)
#define devVerify(expr) (!!(expr))
#define devVerifyMsg(expr, msg) (!!(expr))
#define relAssert(expr) (void)(!!(expr) || _cxAssertFail(#expr, NULL))
#define relAssertMsg(expr, msg) (void)(!!(expr) || _cxAssertFail(#expr, msg))
#define devFatalError(msg) ((void)0)
#define relFatalError(msg) _cxAssertFail(NULL, msg)
#endif

#if defined(DEBUG_LEGACY_ASSERT) && !defined(__INTELLISENSE__)
#define assert(expr) relAssert(expr)
#endif
