#include <cx/debug.h>
#include <string.h>

#define TEST_FILE debugtest
#define TEST_FUNCS debugtest_funcs
#include "common.h"

static int assertcount = 0;
static int asserttest = 0;
static bool assertfail = false;

static int assertCb(const char *expr, const char *msg, const char *file, int line)
{
    assertcount++;

    switch (asserttest) {
    case 0:
        if (strcmp(expr, "0 == 1") != 0 || msg != NULL)
            assertfail = true;
        break;
    case 1:
        if (strcmp(expr, "0 == 1") != 0 || strcmp(msg, "Test 1-2-3") != 0)
            assertfail = true;
        break;
    case 2:
        // we should never get 2 because it's a true expression
        assertfail = true;
        break;
    case 3:
        if (strcmp(expr, "false") != 0 || strcmp(msg, "Dev Only") != 0)
            assertfail = true;
        break;
    case 4:
        if (expr != NULL || strcmp(msg, "OH NO!") != 0)
            assertfail = true;
        break;
    }

    return ASSERT_Ignore;
}

static int test_assert()
{
    int ret = 0;
    int lasttest = 0;

    // install assertion handler so these don't all crash
    dbgAssertAddCallback(assertCb);

    relAssert(0 == 1);
    asserttest++;
    if (assertfail || assertcount != asserttest)
        ret = 1;

    relAssertMsg(0 == 1, "Test 1-2-3");
    asserttest++;
    if (assertfail || assertcount != asserttest)
        ret = 1;

    lasttest = asserttest;
    relAssert(5 == 5);
    asserttest++;
    if (assertfail || assertcount != lasttest)
        ret = 1;
    assertcount = asserttest;           // resync

    if (devVerifyMsg(false, "Dev Only"))
        ret = 1;

    asserttest++;
#if DEBUG_LEVEL > 0
    // Should have asserted in dev / debug builds
    if (assertfail || assertcount != asserttest)
        ret = 1;
#else
    // Should NOT have asserted in release builds
    if (assertfail || assertcount != asserttest - 1)
        ret = 1;
    assertcount++;      // pretend it did
#endif

    relFatalError("OH NO!");
    asserttest++;
    if (assertfail || assertcount != asserttest)
        ret = 1;

    return ret;
}

testfunc debugtest_funcs[] = {
    { "assert", test_assert },
    { 0, 0 }
};
