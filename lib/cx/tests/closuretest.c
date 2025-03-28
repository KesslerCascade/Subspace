#include <stdio.h>
#include <cx/string.h>
#include <cx/closure.h>
#include <cx/string/strtest.h>

#define TEST_FILE closuretest
#define TEST_FUNCS closuretest_funcs
#include "common.h"

typedef struct ClosureTestData
{
    int accum;
    int sref1;
    int sref2;
    int count1;
    int count2;
} ClosureTestData;

static ClosureTestData ctd;

static bool ctest1(stvlist *cvars, stvlist *args)
{
    int n1, n2;
    string s1, s2;
    if(!stvlNext(cvars, int32, &n1) ||
       !stvlNext(cvars, string, &s1) ||
       !stvlNext(args, int32, &n2) ||
       !stvlNext(args, string, &s2))
        return false;

    ctd.accum += n1 + n2;
    ctd.sref1 = strTestRefCount(s1);
    ctd.sref2 = strTestRefCount(s2);
    ctd.count1++;

    return true;
}

static bool ctest2(stvlist *cvars, stvlist *args)
{
    int n1, n2;
    string s1, s2;
    if(!stvlNext(cvars, int32, &n1) ||
       !stvlNext(cvars, string, &s1) ||
       !stvlNext(args, int32, &n2) ||
       !stvlNext(args, string, &s2))
        return false;

    if(!strEq(s1, s2))
        return false;

    ctd.accum += n1 + n2;
    ctd.sref1 = strTestRefCount(s1);
    ctd.sref2 = strTestRefCount(s2);
    ctd.count2++;

    return true;
}

static int test_closuretest_closure(void)
{
    int ret = 0;
    memset(&ctd, 0, sizeof(ctd));

    string teststr1 = 0, teststr2 = 0;
    strCopy(&teststr1, _S"Closure Test String 1");
    strCopy(&teststr2, _S"Closure Test String 2");

    int r1, r2, accum = 0;

    r1 = rand(); r2 = rand();
    accum = r1 + r2;
    closure cls = closureCreate(ctest1, stvar(int32, r1), stvar(string, teststr1));
    if(strTestRefCount(teststr1) != 2)
        ret = 1;

    if(!closureCall(cls, stvar(int32, r2), stvar(string, teststr2)))
        ret = 1;
    if(ctd.count1 != 1 || ctd.sref1 != 2 || ctd.sref2 != 1 || ctd.accum != accum)
        ret = 1;

    closureDestroy(&cls);
    if(strTestRefCount(teststr1) != 1)
        ret = 1;

    r1 = rand(); r2 = rand();
    cls = closureCreate(ctest2, stvar(int32, r1), stvar(string, teststr1));

    // this should fail!
    if(closureCall(cls, stvar(int32, r2), stvar(string, teststr2)))
        ret = 1;
    // and should not change any values
    if(ctd.count1 != 1 || ctd.count2 != 0 || ctd.sref1 != 2 || ctd.sref2 != 1 || ctd.accum != accum)
        ret = 1;

    if(!closureCall(cls, stvar(int32, r2), stvar(string, teststr1)))
        ret = 1;
    accum += r1 + r2;
    if(ctd.count1 != 1 || ctd.count2 != 1 || ctd.sref1 != 2 || ctd.sref2 != 2 || ctd.accum != accum)
        ret = 1;

    closureDestroy(&cls);
    if(strTestRefCount(teststr1) != 1)
        ret = 1;

    strDestroy(&teststr1);
    strDestroy(&teststr2);

    return ret;
}

static int test_closuretest_chain(void)
{
    int ret = 0;
    memset(&ctd, 0, sizeof(ctd));

    string teststr1 = 0, teststr2 = 0;
    strCopy(&teststr1, _S"Closure Test String 1");
    strCopy(&teststr2, _S"Closure Test String 2");

    int r1, r2, r3, r4, accum = 0;

    r1 = rand(); r2 = rand();
    r3 = rand(); r4 = rand();
    accum = r1 + r2;

    cchain cch = NULL;
    cchainAttach(&cch, ctest1, stvar(int32, r1), stvar(string, teststr1));
    if(strTestRefCount(teststr1) != 2)
        ret = 1;

    if(!cchainCall(&cch, stvar(int32, r2), stvar(string, teststr2)))
        ret = 1;
    if(ctd.count1 != 1 || ctd.sref1 != 3 || ctd.sref2 != 1 || ctd.accum != accum)
        ret = 1;

    cchainAttachToken(&cch, ctest2, 17, stvar(int32, r3), stvar(string, teststr1));
    if(strTestRefCount(teststr1) != 3)
        ret = 1;

    // both functions should be called but the overall result should be false
    // and only ctest1 should modify the state
    accum += r1 + r2;
    if(cchainCall(&cch, stvar(int32, r2), stvar(string, teststr2)))
        ret = 1;
    if(ctd.count1 != 2 || ctd.count2 != 0 || ctd.sref1 != 4 || ctd.sref2 != 1 || ctd.accum != accum)
        ret = 1;

    // both functions should be called
    accum += r1 + r2 + r3 + r2;
    if(!cchainCall(&cch, stvar(int32, r2), stvar(string, teststr1)))
        ret = 1;
    if(ctd.count1 != 3 || ctd.count2 != 1 || ctd.sref1 != 4 || ctd.sref2 != 4 || ctd.accum != accum)
        ret = 1;

    // attach a second ctest1
    cchainAttach(&cch, ctest1, stvar(int32, r4), stvar(string, teststr2));
    if(strTestRefCount(teststr2) != 2)
        ret = 1;

    accum += r1 + r2 + r3 + r2 + r4 + r2;
    if(!cchainCall(&cch, stvar(int32, r2), stvar(string, teststr1)))
        ret = 1;
    if(ctd.count1 != 5 || ctd.count2 != 2 || ctd.sref1 != 4 || ctd.sref2 != 4 || ctd.accum != accum)
        ret = 1;

    // detach the ctest2 in the middle
    if(!cchainDetach(&cch, ctest2, 17))
        ret = 1;

    accum += r1 + r2 + r4 + r2;
    if(!cchainCall(&cch, stvar(int32, r2), stvar(string, teststr2)))
        ret = 1;
    if(ctd.count1 != 7 || ctd.count2 != 2 || ctd.sref1 != 3 || ctd.sref2 != 2 || ctd.accum != accum)
        ret = 1;

    cchainDestroy(&cch);
    if(strTestRefCount(teststr1) != 1 || strTestRefCount(teststr2) != 1)
        ret = 1;

    strDestroy(&teststr1);
    strDestroy(&teststr2);

    return ret;
}

testfunc closuretest_funcs[] = {
    { "closure", test_closuretest_closure },
    { "chain", test_closuretest_chain },
    { 0, 0 }
};
