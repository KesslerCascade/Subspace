#include <cx/serialize/streambuf.h>
#include <cx/serialize/sbstring.h>
#include <cx/xalloc/xalloc.h>
#include <cx/string.h>
#include <cx/string/strtest.h>
#include <cx/utils/compare.h>

#define TEST_FILE sbtest
#define TEST_FUNCS sbtest_funcs
#include "common.h"

static const uint8 testdata1[] = "This is a test. This is a test. This is a test. This is a test. This is a test. This is a test.";
#define TESTBUF_SZ 128
_Static_assert(TESTBUF_SZ > sizeof(testdata1), "TESTBUF_SZ must be big enough to hold the test data");

typedef struct TestCtx1 {
    uint8 *out;
    size_t outp;
    size_t shouldread;
    bool didclean;
    bool usesend;
} TestCtx1;

static bool sbsend1(StreamBuffer *sb, const uint8 *buf, size_t off, size_t sz, void *ctx)
{
    TestCtx1 *tc = (TestCtx1 *)ctx;
    if (tc->outp + sz > TESTBUF_SZ)
        return false;

    memcpy(tc->out + tc->outp, buf, sz);
    tc->outp += sz;
    return true;
}

static void sbnotify1(StreamBuffer *sb, size_t sz, void *ctx)
{
    TestCtx1 * tc = (TestCtx1 *)ctx;
    if (tc->outp + sz > TESTBUF_SZ)
        return;

    // test both read and send
    if (!tc->usesend) {
        size_t didread;
        sbufCRead(sb, tc->out + tc->outp, min(sz, tc->shouldread), &didread);
        tc->outp += didread;
        devAssert(didread == min(sz, tc->shouldread));
    } else {
        sbufCSend(sb, sbsend1, min(sz, tc->shouldread));
    }
}

static void sbclean1(void *ctx)
{
    TestCtx1 *tc = (TestCtx1 *)ctx;
    tc->didclean = true;
}

static int test_streambuf_push()
{
    int ret = 0;

    StreamBuffer *ptest;
    TestCtx1 c1 = { 0 };
    c1.out = xaAlloc(TESTBUF_SZ);

    for (int usesend = 0; usesend < 2; usesend++) {
        ptest = sbufCreate(32);
        if (!sbufPRegisterPush(ptest, NULL, 0)) {
            ret = 1;
            goto out;
        }

        if (!sbufCRegisterPush(ptest, sbnotify1, sbclean1, &c1)) {
            ret = 1;
            goto out;
        }

        memset(c1.out, 0, TESTBUF_SZ);
        c1.outp = 0;

        c1.shouldread = 5;
        c1.usesend = !!usesend;
        sbufPWrite(ptest, testdata1, 7);

        // check if data made it
        if (memcmp(c1.out, testdata1, 5))
            ret = 1;

        // check if ring buffer is in expected state
        if (ptest->head != 5 || ptest->tail != 7)
            ret = 1;

        c1.shouldread = 7;
        sbufPWrite(ptest, testdata1 + 9, 5);

        if (memcmp(c1.out, "This is test", 12) ||
            c1.outp != 12)
            ret = 1;

        if (ptest->head != 0 || ptest->tail != 0)
            ret = 1;

        // test overflow buffer
        c1.outp = 0;
        c1.shouldread = 5;
        sbufPWrite(ptest, testdata1, 7);
        c1.shouldread = 1;
        sbufPWrite(ptest, testdata1 + 7, 8);

        if (ptest->head != 6 ||
            ptest->tail != 7 ||
            ptest->overflowtail != 8 ||
            sbufCAvail(ptest) != 9)
            ret = 1;

        c1.shouldread = 5;
        sbufPWrite(ptest, testdata1 + 15, 5);

        // reading 5 should cause the overflow to become the main buffer
        if (ptest->head != 4 ||
            ptest->tail != 13 ||
            ptest->overflowtail != 0)
            ret = 1;

        if (memcmp(c1.out, testdata1, 11) ||
            c1.outp != 11)
            ret = 1;

        // push some more data
        sbufPWrite(ptest, testdata1 + 20, 40);
        if (c1.outp != 16)
            ret = 1;

        // flush everything that's left
        c1.shouldread = 10000;
        sbufPFinish(ptest);
        // this should cause sbclean1 to be called
        sbufRelease(&ptest);

        if (memcmp(c1.out, testdata1, 60) ||
            c1.outp != 60)
            ret = 1;

        if (!c1.didclean)
            ret = 1;
    }

out:
    xaFree(c1.out);
    return ret;
}

typedef struct TestCtx2 {
    size_t inp;
    bool didclean;
} TestCtx2;

static size_t sbpull2(StreamBuffer *sb, uint8 *buf, size_t sz, void *ctx)
{
    TestCtx2 *tc = (TestCtx2 *)ctx;
    size_t bytes = min(sz, sizeof(testdata1) - tc->inp);

    if (sz % 2 == 1) {
        memcpy(buf, testdata1 + tc->inp, bytes);
        tc->inp += bytes;
    } else {
        sbufPWrite(sb, testdata1 + tc->inp, bytes);
        tc->inp += bytes;
        bytes = 0;
    }

    if (tc->inp == sizeof(testdata1))
        sbufPFinish(sb);

    return bytes;
}

static void sbclean2(void *ctx)
{
    TestCtx2 *tc = (TestCtx2 *)ctx;
    tc->didclean = true;
}

static int test_streambuf_pull()
{
    int ret = 0;

    StreamBuffer *ptest = sbufCreate(32);
    TestCtx2 ctx = { 0 };
    uint8 out[TESTBUF_SZ];
    size_t p = 0;

    if (!sbufPRegisterPull(ptest, sbpull2, sbclean2, &ctx))
        return false;

    if (!sbufCRegisterPull(ptest, NULL, NULL))
        return false;

    size_t didread;

    sbufCRead(ptest, out + p, 5, &didread);
    if (didread != 5 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    sbufCRead(ptest, out + p, 15, &didread);
    if (didread != 15 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    sbufCRead(ptest, out + p, 3, &didread);
    if (didread != 3 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    sbufCRead(ptest, out + p, 40, &didread);
    if (didread != 40 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    sbufCRead(ptest, out + p, 13, &didread);
    if (didread != 13 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    // this should hit the end of the input
    sbufCRead(ptest, out + p, 25, &didread);
    if (didread != 20 ||
        memcmp(out + p, testdata1 + p, didread))
        ret = 1;
    p += didread;

    if (p != sizeof(testdata1) ||
        memcmp(out, testdata1, sizeof(testdata1)))
        ret = 1;

    // test releasing in opposite order
    StreamBuffer *temp = ptest;
    sbufRelease(&temp);
    sbufCFinish(ptest);

    if (!ctx.didclean)
        ret = 1;

    return ret;
}

static int test_streambuf_peek()
{
    int ret = 0;

    StreamBuffer *ptest = sbufCreate(32);
    TestCtx2 ctx = { 0 };
    uint8 out[TESTBUF_SZ];
    size_t p = 0, didread;

    if (!sbufPRegisterPull(ptest, sbpull2, sbclean2, &ctx))
        return false;

    if (!sbufCRegisterPull(ptest, NULL, NULL))
        return false;

    if (!sbufCFeed(ptest, 5))
        ret = 1;

    if (!sbufCPeek(ptest, out + p, 0, 5))
        ret = 1;
    if (memcmp(out + p, testdata1 + p, 5))
        ret = 1;
    if (!sbufCSkip(ptest, 5))
        ret = 1;
    p += 5;

    // try peeking at future data
    if (!sbufCFeed(ptest, 15))
        ret = 1;

    if (!sbufCPeek(ptest, out + p + 10, 10, 5))
        ret = 1;
    if (memcmp(out + p + 10, testdata1 + p + 10, 5))
        ret = 1;

    // now fill in the gap

    if (!sbufCRead(ptest, out + p, 10, &didread))
        ret = 1;
    if (memcmp(out + p, testdata1 + p, 10))
        ret = 1;
    p += 10;

    // and skip over what we already read
    if (!sbufCSkip(ptest, 5))
        ret = 1;
    p += 5;

    // populate the whole rest of the buffer
    if (!sbufCFeed(ptest, sizeof(testdata1) - p))
        ret = 1;

    // read the rest out of order
    if (!sbufCPeek(ptest, out + p + 41, 41, 35))
        ret = 1;
    if (memcmp(out + p + 41, testdata1 + p + 41, 35))
        ret = 1;

    if (!sbufCPeek(ptest, out + p, 0, 41))
        ret = 1;
    if (memcmp(out + p, testdata1 + p, 41))
        ret = 1;

    // skip to the end
    if (!sbufCSkip(ptest, 76))
        ret = 1;
    p += 76;

    // check entire buffer
    if (p != sizeof(testdata1) ||
        memcmp(out, testdata1, sizeof(testdata1)))
        ret = 1;

    sbufCFinish(ptest);
    sbufRelease(&ptest);

    if (!ctx.didclean)
        ret = 1;

    return ret;
}

typedef struct TestCtx3 {
    uint8 *out;
    size_t outp;
    bool didclean;
} TestCtx3;

static void sbpush3(StreamBuffer *sb, const uint8 *buf, size_t sz, void *ctx)
{
    TestCtx3 *tc = (TestCtx3 *)ctx;

    if (tc->outp + sz > TESTBUF_SZ)
        return;

    memcpy(tc->out + tc->outp, buf, sz);
    tc->outp += sz;
}

static void sbclean3(void *ctx)
{
    TestCtx3 *tc = (TestCtx3 *)ctx;
    tc->didclean = true;
}

static int test_streambuf_direct()
{
    int ret = 0;
    StreamBuffer *ptest;
    TestCtx3 c3 = { 0 };
    c3.out = xaAlloc(TESTBUF_SZ);
    ptest = sbufCreate(0);
    if (!sbufPRegisterPush(ptest, NULL, 0)) {
        ret = 1;
        goto out;
    }

    if (!sbufCRegisterPushDirect(ptest, sbpush3, sbclean3, &c3)) {
        ret = 1;
        goto out;
    }

    sbufPWrite(ptest, testdata1, 7);

    // check if data made it
    if (memcmp(c3.out, testdata1, 7))
        ret = 1;

    sbufPWrite(ptest, testdata1 + 7, 5);

    if (memcmp(c3.out, testdata1, 12) ||
        c3.outp != 12)
        ret = 1;

    sbufPWrite(ptest, testdata1 + 12, sizeof(testdata1) - 12);
    if (memcmp(c3.out, testdata1, sizeof(testdata1)) ||
        c3.outp != sizeof(testdata1))
        ret = 1;

    sbufPFinish(ptest);
    // this should cause sbclean3 to be called
    sbufRelease(&ptest);

    if (!c3.didclean)
        ret = 1;

out:
    xaFree(c3.out);
    return ret;
}

static int test_streambuf_string()
{
    int ret = 0;
    string s1 = 0, s2 = 0;
    uint8 buf[128];
    size_t didread;
    strCopy(&s1, _S"This is a string test... This is a string test... This is a string test...");

    StreamBuffer *ptest;
    ptest = sbufCreate(16);
    if (!sbufCRegisterPull(ptest, NULL, NULL) ||
        !sbufStrPRegisterPull(ptest, s1))
        return 1;

    if (strTestRefCount(s1) != 2)
        ret = 1;

    sbufCRead(ptest, buf, 12, &didread);
    if (memcmp(buf, strC(s1), 12))
        ret = 1;

    sbufCRead(ptest, buf, 40, &didread);
    if (memcmp(buf, strC(s1) + 12, 40))
        ret = 1;

    sbufCRead(ptest, buf, 22, &didread);
    if (didread != 22)
        ret = 1;
    if (memcmp(buf, strC(s1) + 52, 22))
        ret = 1;

    sbufCFinish(ptest);
    if (!sbufIsPFinished(ptest))
        ret = 1;

    sbufRelease(&ptest);

    // hook up a string consumer and use the string producer to push to it

    ptest = sbufCreate(16);
    if (!sbufStrCRegisterPush(ptest, &s2))
        return 1;
    sbufStrIn(ptest, s1);

    if (!strEq(s1, s2))
        ret = 1;

    if (!sbufIsPFinished(ptest) || !sbufIsCFinished(ptest))
        ret = 1;

    sbufRelease(&ptest);
    strDestroy(&s2);

    // then the reverse

    ptest = sbufCreate(16);
    if (!sbufStrPRegisterPull(ptest, s1))
        return 1;
    sbufStrOut(ptest, &s2);
    if (!strEq(s1, s2))
        ret = 1;

    if (!sbufIsPFinished(ptest) || !sbufIsCFinished(ptest))
        ret = 1;

    sbufRelease(&ptest);
    strDestroy(&s2);

    strDestroy(&s1);
    return ret;
}

testfunc sbtest_funcs[] = {
    { "push", test_streambuf_push },
    { "pull", test_streambuf_pull },
    { "direct", test_streambuf_direct },
    { "peek", test_streambuf_peek },
    { "string", test_streambuf_string },
    { 0, 0 }
};
