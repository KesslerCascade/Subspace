#include "cx/cx.h"
#include "cx/debug/assert.h"
#include <stdio.h>

#if DEBUG_LEVEL >= 1
_Use_decl_annotations_
CX_C _no_inline bool _cxAssertFail(const char *expr, const char *msg, const char *file, int ln){
    printf("Assertion failure!\n");
    if (msg)
        printf("%s\n", msg);
    if (expr)
        printf("expr: %s\n", expr);
    if (file)
        printf("%s:%d\n", file, ln);
    exit(1);
}
#else
_Use_decl_annotations_
CX_C _no_inline bool _cxAssertFail(const char *expr, const char *msg) { exit(1); }
#endif

_no_inline _no_return void dbgCrashNow(int skip)
{
#if defined(COMPLIER_CLANG) || defined(COMPILER_GCC)
    __builtin_trap();
#else
    volatile char *badptr = 0;
    _Analysis_assume_(badptr != NULL);
    *badptr = 0;        // NOLINT
#endif
    exit(-1);
}

intptr stCmp_suid(stype st, stgeneric stgen1, stgeneric stgen2, uint32 flags) { return 0; }

_Success_(return) _Check_return_
bool stConvert_suid(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags)
{ return false; }

_Use_decl_annotations_
bool suidDecode(SUID *out, strref str) { return false; }
