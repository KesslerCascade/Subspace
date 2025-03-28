#include <stdio.h>
#include <cx/meta.h>
#include <cx/string.h>

#define TEST_FILE metatest
#define TEST_FUNCS metatest_funcs
#include "common.h"

static int test_meta_wrap()
{
    int ret = 0;

    // volatile because the MSVC optimizer deduced that the result of the loops is in fact
    // predetermined and cannot possibly have any other result, so it reduced the entire function
    // to just "return 0;"
    // While that's a good proof that the logic is correct, volatile makes the test suite actually
    // go through the motions and execute it.
    volatile int tvar1, tvar2;

    // test basic block wrapping
    tvar1 = tvar2 = 0;
    blkWrap(tvar1 = 17, tvar2 = tvar2 / 4)
    {
        tvar1 *= 2;
        tvar2 += 28;
    }
    if (tvar1 != 34 || tvar2 != 7)
        ret = 1;

    tvar1 = tvar2 = 0;
    blkWrap(tvar1 = 17, tvar2 = tvar2 / 4)
    {
        tvar1 *= 2;
        tvar2 += 28;
        tvar1 *= 2;
        tvar2 += 28;
    }
    if (tvar1 == 34 || tvar2 == 7)
        ret = 1;

    // test 'continue' to break out
    tvar1 = tvar2 = 0;
    blkWrap(tvar1 = 17, tvar2 = tvar2 / 4)
    {
        tvar1 *= 2;
        tvar2 += 28;
        continue;
        tvar1 *= 2;
        tvar2 += 28;
    }
    if (tvar1 != 34 || tvar2 != 7)
        ret = 1;

    // test 'break' to break out
    tvar1 = tvar2 = 0;
    blkWrap(tvar1 = 17, tvar2 = tvar2 / 4)
    {
        tvar1 *= 2;
        tvar2 += 28;
        break;
        tvar1 *= 2;
        tvar2 += 28;
    }
    if (tvar1 != 34 || tvar2 != 7)
        ret = 1;

    // nested blocks
    tvar1 = tvar2 = 0;
    blkWrap(tvar1 = 17, tvar2 = tvar2 / 4)
    {
        tvar1 *= 2;
        tvar2 += 28;
        blkWrap(tvar1 += 7, (tvar2 += 16, tvar1++))
        {
            tvar1 -= 32;
            tvar2 *= 2;
            continue;
            tvar1 -= 32;
            tvar2 *= 2;
        }
        break;
        tvar1 *= 2;
        tvar2 += 28;
    }
    if (tvar1 != 10 || tvar2 != 18)
        ret = 1;

    // This compiles incorrectly on some older MSVC versions and results in an infinite loop
    // if return inhibit checking is enabled. Add this to the test suite to make sure we catch
    // it if the bug reoccurs at some point.
    // TODO: Possibly move this to another thread so the test can fail rather than just hang.
    int switchvar = 2;
    switch (switchvar) {
    case 1:
        return 1;
    case 2:
        return ret;
    case 3:
        return 1;
    }
    return 1;
}

static int test_meta_pblock_basic(string *pout)
{
    // check basic protected block
    strClear(pout);
    pblock{
        strAppend(pout, _S"A");
PBLOCK_AFTER:
        strAppend(pout, _S"B");
    }

    if (!strEq(*pout, _S"AB"))
        return 1;

    return 0;
}

static int test_meta_pblock_unwind(string *pout)
{
    // basic unwind test
    strClear(pout);
    pblock {
        strAppend(pout, _S"A");
        pblockUnwind(-1);
        strAppend(pout, _S"B");
PBLOCK_AFTER:
        strAppend(pout, _S"C");
    };

    if (!strEq(*pout, _S"AC"))
        return 1;

    return 0;
}

static void test_meta_pblock_unwind_nested(string *pout, int depth, int unwind)
{
    strClear(pout);
    pblock{
        strAppend(pout, _S"A1");
        pblock {
            strAppend(pout, _S"B1");
            pblock {
                strAppend(pout, _S"C1");
                pblock {
                    strAppend(pout, _S"D1");
                    pblock{
                        strAppend(pout, _S"E1");
                        pblock {
                            strAppend(pout, _S"F1");
                            pblock {
                                strAppend(pout, _S"G1");
                                if (depth == 7) pblockUnwind(unwind);
                                strAppend(pout, _S"G2");
                                PBLOCK_AFTER: strAppend(pout, _S"G3");
                            };
                            if (depth == 6) pblockUnwind(unwind);
                            strAppend(pout, _S"F2");
                            PBLOCK_AFTER: strAppend(pout, _S"F3");
                        };
                        if (depth == 5) pblockUnwind(unwind);
                        strAppend(pout, _S"E2");
                        PBLOCK_AFTER: strAppend(pout, _S"E3");
                    };
                    if (depth == 4) pblockUnwind(unwind);
                    strAppend(pout, _S"D2");
                    PBLOCK_AFTER: strAppend(pout, _S"D3");
                };
                if (depth == 3) pblockUnwind(unwind);
                strAppend(pout, _S"C2");
                PBLOCK_AFTER: strAppend(pout, _S"C3");
            };
            if (depth == 2) pblockUnwind(unwind);
            strAppend(pout, _S"B2");
            PBLOCK_AFTER: strAppend(pout, _S"B3");
        };
        if (depth == 1) pblockUnwind(unwind);
        strAppend(pout, _S"A2");
        PBLOCK_AFTER: strAppend(pout, _S"A3");
    };
}

// "A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A2A3"

static int test_meta_protect()
{
    int ret = 0;
    string out = 0;

    ret |= test_meta_pblock_basic(&out);
    ret |= test_meta_pblock_unwind(&out);

    test_meta_pblock_unwind_nested(&out, 0, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 1, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 2, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 3, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 4, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 5, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E3D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 6, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F3E3D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 7, -1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G3F3E3D3C3B3A3")) ret = 1;

    test_meta_pblock_unwind_nested(&out, 0, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 1, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 2, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 3, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 4, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 5, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E3D2D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 6, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F3E2E3D2D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 7, 1);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G3F2F3E2E3D2D3C2C3B2B3A2A3")) ret = 1;


    test_meta_pblock_unwind_nested(&out, 0, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 1, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B2B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 2, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C2C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 3, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D2D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 4, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E2E3D3C3B3A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 5, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F2F3E3D3C3B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 6, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G2G3F3E3D3C3B2B3A2A3")) ret = 1;
    test_meta_pblock_unwind_nested(&out, 7, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1G1G3F3E3D3C2C3B2B3A2A3")) ret = 1;

    strDestroy(&out);
    return ret;
}

static int test_meta_ptry()
{
    volatile int var1 = 0, var2 = 0, var3 = 0;
    int ret = 0;

    // test try/finally/catch blocks with no exceptions
    ptTry {
        var1 += 1;
        ptTry {
            var1 += 10;
            var2 += 20;
        } ptFinally {
            var3 += 30;
        }
        var2 += 2;
    } ptCatch {
        if (ptCode == -7)
            var3 += 3;
    }

    if (var1 != 11 || var2 != 22 || var3 != 30)
        ret = 1;

    // test try/finally/catch blocks with an exception in the inner block
    var1 = var2 = var3 = 0;
    ptTry {
        var1 += 1;
        ptTry {
            var1 += 10;
            ptThrow(-7, "test");
            var2 += 20;
        } ptFinally {
            var3 += 30;
        }
        var2 += 2;
    } ptCatch {
        if (ptCode == -7)
            var3 += 3;
    }

    if (var1 != 11 || var2 != 0 || var3 != 33)
        ret = 1;

    // test try/finally/catch blocks with an exception caught in the inner block
    var1 = var2 = var3 = 0;
    ptTry{
        var1 += 1;
        ptTry {
            var1 += 10;
            ptThrow(-7, "test");
            var2 += 20;
        } ptCatch {
            if (ptCode == -7)
                var3 += 30;
        }
        var2 += 2;
    } ptCatch {
        if (ptCode == -7)
            var3 += 3;
    }

    if (var1 != 11 || var2 != 2 || var3 != 30)
        ret = 1;

    return ret;
}

static int test_meta_ptry_rethrow()
{
    volatile int var1 = 0, var2 = 0, var3 = 0;
    int ret = 0;

    ptTry{
        var1 += 1;
        ptTry {
            var1 += 10;
            ptThrow(-7, "test");
            var2 += 20;
        } ptCatch {
            if (ptCode == -7) {
                var3 += 30;
                ptRethrow;
            }
        }
        var2 += 2;
    } ptCatch {
        if (ptCode == -7)
            var3 += 3;
    }

    if (var1 != 11 || var2 != 0 || var3 != 33)
        ret = 1;

    return ret;
}

static bool unhandled = false;

static int test_meta_ptry_unhandled_handler(ExceptionInfo *einfo)
{
    unhandled = true;

    if (einfo->code == -7 || einfo->code == 1)
        return 1;

    return 0;
}

static int test_meta_ptry_unhandled()
{
    volatile int var1 = 0, var2 = 0, var3 = 0;
    int ret = 0;

    ptRegisterUnhandled(test_meta_ptry_unhandled_handler);
    unhandled = false;

    // properly handled exception
    ptTry{
        var1 += 1;
        ptTry {
            var1 += 10;
            ptThrow(-7, "test");
            var2 += 20;
        } ptFinally {
            var3 += 30;
        }
        var2 += 2;
    } ptCatch {
        if (ptCode == -7)
            var3 += 3;
    }

    if (var1 != 11 || var2 != 0 || var3 != 33 || unhandled)
        ret = 1;

    var1 = var2 = var3 = 0;
    unhandled = false;
    // unhandled exception!
    ptTry {
        var1 += 1;
        ptTry {
            var1 += 10;
            ptThrow(-7, "test");
            var2 += 20;
        } ptFinally {
            var3 += 30;
        }
        var2 += 2;
    } ptFinally {
        var3 += 3;
    }

    if (var1 != 11 || var2 != 0 || var3 != 33 || !unhandled)
        ret = 1;

    ptUnregisterUnhandled(test_meta_ptry_unhandled_handler);

    return ret;
}

#define TEST_XFUNC_RECURSE test_meta_ptry_xfunc_sub(pout, level + 1, p2, p3, p4, p5, p6, 0)

static void test_meta_ptry_xfunc_sub(string *pout, int level, int p1, int p2, int p3, int p4, int p5, int p6)
{
    char t1[2] = { 0 };
    t1[0] = 'A' + level;

    strNConcat(pout, *pout, (strref)t1, _S"1");

    if (p1 == 0) {
        // no recursion
    } else if (p1 == 1) {
        // recurse outside of a try block
        TEST_XFUNC_RECURSE;
        strNConcat(pout, *pout, (strref)t1, _S"2");
    } else if (p1 == 2) {
        // recurse inside a try block, catch an exception at this level
        ptTry {
            TEST_XFUNC_RECURSE;
            strNConcat(pout, *pout, (strref)t1, _S"2");
        } ptCatch {
            if (ptCode == 1) {
                strNConcat(pout, *pout, (strref)t1, _S"c");
            } else if (ptCode == 2) {
                strNConcat(pout, *pout, (strref)t1, _S"z");
            }
            strNConcat(pout, *pout, (strref)t1, _S"3");
        }
    } else if (p1 == 3) {
        // recurse inside a try block, do NOT catch an exception at this level
        ptTry {
            TEST_XFUNC_RECURSE;
            strNConcat(pout, *pout, (strref)t1, _S"2");
        } ptFinally {
            strNConcat(pout, *pout, (strref)t1, _S"3");
        }
    } else if (p1 == 4) {
        // throw an exception at this level
        strNConcat(pout, *pout, (strref)t1, _S"e");
        ptThrow(1, "test");
        TEST_XFUNC_RECURSE;
        strNConcat(pout, *pout, (strref)t1, _S"2");
    } else if (p1 == 5) {
        // recurse inside a try block, catch an exception at this level,
        // but throw a different exception inside the catch
        ptTry {
            TEST_XFUNC_RECURSE;
            strNConcat(pout, *pout, (strref)t1, _S"2");
        } ptCatch {
            if (ptCode == 1) {
                strNConcat(pout, *pout, (strref)t1, _S"c");
            } else if (ptCode == 2) {
                strNConcat(pout, *pout, (strref)t1, _S"z");
            }
            strNConcat(pout, *pout, (strref)t1, _S"e");
            ptThrow(2, "test");
            strNConcat(pout, *pout, (strref)t1, _S"3");
        }
    }

    if (unhandled)
        return;

    strNConcat(pout, *pout, (strref)t1, _S"4");
}

static int test_meta_ptry_xfunc()
{
    string out = 0;
    int ret = 0;
    unhandled = false;

    // test basic recursion logic
    test_meta_ptry_xfunc_sub(&out, 0, 1, 1, 1, 0, 0, 0);
    if (!strEq(out, _S"A1B1C1D1D4C2C4B2B4A2A4"))
        ret = 1;

    // throwing an exception from several levels deep in the call chain
    strClear(&out);
    test_meta_ptry_xfunc_sub(&out, 0, 2, 1, 1, 3, 1, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1FeD3AcA3A4"))
        ret = 1;

    // catching an exception at a lower level
    strClear(&out);
    test_meta_ptry_xfunc_sub(&out, 0, 2, 1, 3, 2, 1, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1FeDcD3D4C2C3C4B2B4A2A3A4"))
        ret = 1;

    // making sure execution terminates at the exception
    strClear(&out);
    test_meta_ptry_xfunc_sub(&out, 0, 2, 1, 4, 1, 4, 2);
    if (!strEq(out, _S"A1B1C1CeAcA3A4"))
        ret = 1;

    // check unhandled exceptions in a function chain
    ptRegisterUnhandled(test_meta_ptry_unhandled_handler);
    unhandled = false;
    strClear(&out);
    test_meta_ptry_xfunc_sub(&out, 0, 3, 1, 3, 1, 4, 0);
    if (!strEq(out, _S"A1B1C1D1E1EeC3A3") || !unhandled)
        ret = 1;
    ptUnregisterUnhandled(test_meta_ptry_unhandled_handler);

    // throwing an exception in a catch block
    unhandled = false;
    strClear(&out);
    test_meta_ptry_xfunc_sub(&out, 0, 2, 1, 3, 5, 1, 4);
    if (!strEq(out, _S"A1B1C1D1E1F1FeDcDeC3AzA3A4"))
        ret = 1;

    strDestroy(&out);

    return ret;
}

testfunc metatest_funcs[] = {
    { "wrap", test_meta_wrap },
    { "protect", test_meta_protect },
    { "ptry", test_meta_ptry },
    { "unhandled", test_meta_ptry_unhandled },
    { "rethrow", test_meta_ptry_rethrow },
    { "xfunc", test_meta_ptry_xfunc },
    { 0, 0 }
};
