#include <cx/string.h>
#include <cx/string/strtest.h>

#define TEST_FILE strtest
#define TEST_FUNCS strtest_funcs
#include "common.h"

static int test_join()
{
    string t1 = _S"Test 1";
    string t2 = _S"Test 2";
    string t3 = _S"Test 3";

    string o = NULL;

    strConcat(&o, t1, t2);
    if (!strEq(o, _S"Test 1Test 2"))
        return 1;

    strNConcat(&o, t1, t2, t3);
    if (!strEq(o, _S"Test 1Test 2Test 3"))
        return 1;

    string s1 = NULL, s2 = NULL, s3 = NULL;
    strDup(&s1, t1);
    strDup(&s2, t2);
    strDup(&s3, t3);
    strNConcatC(&o, &s3, &s2, &s1);
    if (!strEq(o, _S"Test 3Test 2Test 1"))
        return 1;
    if (s1 != NULL || s2 != NULL || s3 != NULL)
        return 1;

    strDestroy(&o);
    return 0;
}

static int test_append()
{
    string t1 = _S"Test 1";
    string pfx = _S"Pre-";
    string sfx = _S"-ish";

    strPrepend(pfx, &t1);
    if (!strEq(t1, _S"Pre-Test 1"))
        return 1;

    strAppend(&t1, sfx);
    if (!strEq(t1, _S"Pre-Test 1-ish"))
        return 1;

    strDestroy(&t1);
    return 0;
}

static int test_substr()
{
    string t1 = _S"Relatively long substring test data";
    string o = NULL;

    strSubStr(&o, t1, 0, 6);
    if (!strEq(o, _S"Relati"))
        return 1;

    strSubStr(&o, t1, 11, 15);
    if (!strEq(o, _S"long"))
        return 1;

    strSubStr(&o, t1, 16, 25);
    if (!strEq(o, _S"substring"))
        return 1;

    strSubStr(&o, t1, 16, -5);
    if (!strEq(o, _S"substring test"))
        return 1;

    strSubStr(&o, t1, -9, -5);
    if (!strEq(o, _S"test"))
        return 1;

    strSubStr(&o, t1, -4, strEnd);
    if (!strEq(o, _S"data"))
        return 1;

    string s1 = NULL;
    strDup(&s1, t1);
    strSubStrC(&o, &s1, 11, 15);
    if (!strEq(o, _S"long"))
        return 1;
    if (s1 != NULL)
        return 1;

    strSubStr(&o, t1, 0, -5);
    if (!strEq(o, _S"Relatively long substring test"))
        return 1;

    strSubStrI(&o, -11, strEnd);
    if (!strEq(o, _S"string test"))
        return 1;

    strSubStr(&o, t1, 0, -5);
    strSubStrI(&o, 0, 4);
    if (!strEq(o, _S"Rela"))
        return 1;

    strDestroy(&o);
    return 0;
}

static int test_long()
{
    string o1 = NULL, o2 = NULL;
    string t1 = _S"Relatively long substring test data";       // 35 characters
    string s1 = NULL;

    strReset(&o1, 100);           // sizehint here is intentionally a LIE ;)
    strReset(&o2, 1);

    for (int i = 0; i < 1000; ++i) {
        strAppend(&o1, t1);
    }

    if (strLen(o1) != 35000)
        return 1;

    for (int i = 0; i < 1000; ++i) {
        strAppend(&o2, o1);
    }

    if (strLen(o2) != 35000000)
        return 1;

    strSubStr(&s1, o2, 7392013, 7392026);
    if (!strEq(s1, _S"ng substring "))
        return 1;

    strDestroy(&o1);
    strDestroy(&o2);
    strDestroy(&s1);
    return 0;
}

static int test_compare()
{
    string t1 = _S"Test 1";
    string t2 = _S"Test 2";
    string t1l = _S"Test 1 Long";
    string t2l = _S"Test 2 Long";
    string lsfx = _S" Long";

    if (strCmp(t1, t2) >= 0)
        return 1;

    if (strCmp(t1, t1l) >= 0)
        return 1;

    if (strCmp(t2l, t2) <= 0)
        return 1;

    if (strCmp(t1, t2l) >= 0)
        return 1;

    if (strCmp(t2, t1l) <= 0)
        return 1;

    string t2l2 = NULL;
    strConcat(&t2l2, t2, lsfx);

    if (strCmp(t2l, t2l2) != 0)
        return 1;

    strDestroy(&t2l2);

    return 0;
}

static int test_rope()
{
    string t1 = _S"Thirty-two character test string";
    string t2 = _S"gnirts tset retcarahc owt-ytrihT";
    string s1 = NULL, s2 = NULL;
    string o1 = NULL, o2 = NULL, o3 = NULL;
    int i;

    strNConcat(&s1, t1, t2);
    strAppend(&o1, s1);
    strAppend(&o1, s1);

    if (strTestRopeDepth(o1) != 1)
        return 1;

    if (!strTestRopeNode(&o2, o1, true) || strLen(o2) != 64)
        return 1;
    if (!strTestRopeNode(&o2, o1, false) || strLen(o2) != 64)
        return 1;

    strDestroy(&o1);
    for (i = 0; i < 32; i++) {
        strAppend(&o1, s1);
    }

    if (strTestRefCount(s1) < 32)
        return 1;

    if (strTestRopeDepth(o1) != 5)
        return 1;

    if (!strTestRopeNode(&o2, o1, true) || strLen(o2) != 1024)
        return 1;
    if (!strTestRopeNode(&o2, o1, false) || strLen(o2) != 1024)
        return 1;

    strDestroy(&o1);
    // pathalogically build up a huge rope
    for (i = 0; i < 10000; i++) {
        strAppend(&o1, _S"b");
        strPrepend(_S"a", &o1);
    }

    // make sure it's not horribly unbalanced even in worst case
    if (strTestRopeDepth(o1) > 9)
        return 1;

    if (!strTestRopeNode(&o2, o1, true) || strLen(o2) < 8000)
        return 1;
    if (!strTestRopeNode(&o2, o1, false) || strLen(o2) < 8000)
        return 1;

    strDestroy(&o1);
    for (i = 0; i < 1024; i++) {
        strAppend(&o1, s1);
    }

    strSubStr(&s2, o1, 10000, 15000);
    strSubStr(&o2, s2, 0, 10);
    if (!strEq(o2, _S"cter test "))
        return 1;

    strNConcat(&o2, s1, s2, s2, s1);
    if (strLen(o2) != 10128)
        return 1;

    strSubStr(&o3, o2, 60, 70);
    if (!strEq(o3, _S"rihTcter t"))
        return 1;
    strSubStr(&o3, o2, 5059, 5069);
    if (!strEq(o3, _S"r tescter "))
        return 1;

    // manually fill in a buffer to avoid creating a rope.
    // need a single string to avoid rope segment optimizations or it
    // gets a lot harder to check the refcount.
    strReset(&s2, 1000);
    uint8 *buf = strBuffer(&s2, 1000);
    for (i = 0; i < 1000; i += 25) {
        memcpy(&buf[i], "1234567890123456789012345", 25);
    }
    strSubStr(&o1, s2, 250, 750);

    if (strTestRefCount(s2) < 2)
        return 1;

    strNConcat(&o2, o1, _S"Test 123", o1);

    if (strTestRefCount(s2) < 4)
        return 1;
    if (strLen(o2) != 1008)
        return 1;

    strDestroy(&o1);
    strDestroy(&o2);
    strDestroy(&o3);

    if (strTestRefCount(s2) != 1)
        return 1;

    strDestroy(&s1);
    strDestroy(&s2);

    return 0;
}

testfunc strtest_funcs[] = {
    { "join", test_join },
    { "append", test_append },
    { "substr", test_substr },
    { "compare", test_compare },
    { "longstring", test_long },
    { "rope", test_rope },
    { 0, 0 }
};
