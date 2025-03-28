#include <cx/serialize/streambuf.h>
#include <cx/serialize/sbstring.h>
#include <cx/serialize/lineparse.h>
#include <cx/string.h>
#include <cx/string/strtest.h>
#include <cx/utils/compare.h>

#define TEST_FILE lineparsetest
#define TEST_FUNCS lineparsetest_funcs
#include "common.h"

static const char testdata_lf[] = "This is a test of the lineparser code. This is line 1.\n"
"This is line 2.\n"
"This is line 3.\n"
"This is line 4.\n"
"This is line 5.\n"
"This is line 6.\n"
"This is line 7.\n"
"This is line 8.\n"
"This is line 9.\n"
"This is line 10.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\n"
"(99) THIS IS THE NEXT TO LAST LINE!\n"
"(100) THIS IS THE LAST LINE!"
;

static const char testdata_crlf[] = "This is a test of the lineparser code. This is line 1.\r\n"
"This is line 2.\r\n"
"This is line 3.\r\n"
"This is line 4.\r\n"
"This is line 5.\r\n"
"This is line 6.\r\n"
"This is line 7.\r\n"
"This is line 8.\r\n"
"This is line 9.\r\n"
"This is line 10.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\r\n"
"(99) THIS IS THE NEXT TO LAST LINE!\r\n"
"(100) THIS IS THE LAST LINE!\r\n"
;

static const char testdata_mixed1[] = "This is a test of the lineparser code. This is line 1.\n"
"This is line 2.\r\n"
"This is line 3.\n"
"This is line 4.\r\n"
"This is line 5.\n"
"This is line 6.\r\n"
"This is line 7.\n"
"This is line 8.\r\n"
"This is line 9.\n"
"This is line 10.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"(99) THIS IS THE NEXT TO LAST LINE!\n"
"(100) THIS IS THE LAST LINE!"
;

static const char testdata_mixed2[] = "This is a test of the lineparser code. This is line 1.\r\n"
"This is line 2.\n"
"This is line 3.\r\n"
"This is line 4.\n"
"This is line 5.\r\n"
"This is line 6.\n"
"This is line 7.\r\n"
"This is line 8.\n"
"This is line 9.\r\n"
"This is line 10.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"Up to 90 more lines may follow.\r\n"
"Up to 90 more lines may follow.\n"
"(99) THIS IS THE NEXT TO LAST LINE!\r\n"
"(100) THIS IS THE LAST LINE!"
;

static string line1 = _S"This is a test of the lineparser code. This is line 1.";
static string line5 = _S"This is line 5.";
static string line8 = _S"This is line 8.";
static string line9 = _S"This is line 9.";
static string linerepeat = _S"Up to 90 more lines may follow.";
static string line99 = _S"(99) THIS IS THE NEXT TO LAST LINE!";
static string line100 = _S"(100) THIS IS THE LAST LINE!";

int test_lineparse_explicit()
{
    int ret = 0;
    string teststr_lf = 0;
    string teststr_crlf = 0;

    strCopy(&teststr_lf, (strref)testdata_lf);
    strCopy(&teststr_crlf, (strref)testdata_crlf);

    StreamBuffer *sb;
    int lines;
    string line = 0;

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_lf) ||
        !lparseRegisterPull(sb, LPARSE_LF))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // retest with NoIncomplete
    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_lf) ||
        !lparseRegisterPull(sb, LPARSE_LF | LPARSE_NoIncomplete))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
    }

    if (lines != 99)
        ret = 1;

    sbufRelease(&sb);

    // CRLF

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_crlf) ||
        !lparseRegisterPull(sb, LPARSE_CRLF))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // retest with NoIncomplete
    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_crlf) ||
        !lparseRegisterPull(sb, LPARSE_CRLF | LPARSE_NoIncomplete))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // finally a quick test with includeeol

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_crlf) ||
        !lparseRegisterPull(sb, LPARSE_CRLF | LPARSE_IncludeEOL))
        return 1;
    string temp = 0;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        strConcat(&temp, line1, _S"\r\n");
        if (lines == 1 && !strEq(line, temp))
            ret = 1;
        strConcat(&temp, line5, _S"\r\n");
        if (lines == 5 && !strEq(line, temp))
            ret = 1;
        strConcat(&temp, linerepeat, _S"\r\n");
        if (lines > 10 && lines < 99 && !strEq(line, temp))
            ret = 1;
        strConcat(&temp, line99, _S"\r\n");
        if (lines == 99 && !strEq(line, temp))
            ret = 1;
        strConcat(&temp, line100, _S"\r\n");
        if (lines == 100 && !strEq(line, temp))
            ret = 1;
    }
    strDestroy(&temp);

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    strDestroy(&line);
    strDestroy(&teststr_lf);
    strDestroy(&teststr_crlf);

    return ret;
}

int test_lineparse_auto()
{
    int ret = 0;
    string teststr_lf = 0;
    string teststr_crlf = 0;
    string teststr_mixed1 = 0;
    string teststr_mixed2 = 0;

    strCopy(&teststr_lf, (strref)testdata_lf);
    strCopy(&teststr_crlf, (strref)testdata_crlf);
    strCopy(&teststr_mixed1, (strref)testdata_mixed1);
    strCopy(&teststr_mixed2, (strref)testdata_mixed2);

    StreamBuffer *sb;
    int lines;
    string line = 0;

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_lf) ||
        !lparseRegisterPull(sb, LPARSE_Auto))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // CRLF

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_crlf) ||
        !lparseRegisterPull(sb, LPARSE_Auto))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // Mixed 1 (should detect CR)
    string temp = 0;

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_mixed1) ||
        !lparseRegisterPull(sb, LPARSE_Auto))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        strConcat(&temp, line8, _S"\r");
        if (lines == 8 && !strEq(line, temp))
            ret = 1;
        if (lines > 10 && lines < 99 && (lines % 2) == 1 && !strEq(line, linerepeat))
            ret = 1;
        strConcat(&temp, linerepeat, _S"\r");
        if (lines > 10 && lines < 99 && (lines % 2) == 0 && !strEq(line, temp))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // Mixed 1 (should detect CRLF)

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_mixed2) ||
        !lparseRegisterPull(sb, LPARSE_Auto))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        strNConcat(&temp, line8, _S"\n", line9);
        if (lines == 5 && !strEq(line, temp))
            ret = 1;
        strNConcat(&temp, linerepeat, _S"\n", linerepeat);
        if (lines > 6 && lines < 48 && !strEq(line, temp))
            ret = 1;
        if (lines == 51 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 51)
        ret = 1;

    sbufRelease(&sb);

    strDestroy(&temp);
    strDestroy(&line);
    strDestroy(&teststr_lf);
    strDestroy(&teststr_crlf);
    strDestroy(&teststr_mixed1);
    strDestroy(&teststr_mixed2);

    return ret;
}

int test_lineparse_mixed()
{
    int ret = 0;
    string teststr_mixed1 = 0;
    string teststr_mixed2 = 0;

    strCopy(&teststr_mixed1, (strref)testdata_mixed1);
    strCopy(&teststr_mixed2, (strref)testdata_mixed2);

    StreamBuffer *sb;
    int lines;
    string line = 0;

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_mixed1) ||
        !lparseRegisterPull(sb, LPARSE_Mixed))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    // Mixed2

    sb = sbufCreate(512);
    if (!sbufStrPRegisterPull(sb, teststr_mixed2) ||
        !lparseRegisterPull(sb, LPARSE_Mixed))
        return 1;

    lines = 0;
    while (lparseLine(sb, &line)) {
        lines++;
        if (lines == 1 && !strEq(line, line1))
            ret = 1;
        if (lines == 5 && !strEq(line, line5))
            ret = 1;
        if (lines > 10 && lines < 99 && !strEq(line, linerepeat))
            ret = 1;
        if (lines == 99 && !strEq(line, line99))
            ret = 1;
        if (lines == 100 && !strEq(line, line100))
            ret = 1;
    }

    if (lines != 100)
        ret = 1;

    sbufRelease(&sb);

    strDestroy(&line);
    strDestroy(&teststr_mixed1);
    strDestroy(&teststr_mixed2);

    return ret;
}

typedef struct LineParsePushTestCtx {
    int ret;
    int lines;
    bool didclean;
} LineParsePushTestCtx;

static void test_ctxcleanup(void *ctx)
{
    LineParsePushTestCtx *lppt = (LineParsePushTestCtx *)ctx;
    lppt->didclean = true;
}

static bool test_linecb(strref line, void *ctx)
{
    LineParsePushTestCtx *lppt = (LineParsePushTestCtx *)ctx;

    lppt->lines++;
    if (lppt->lines == 1 && !strEq(line, line1))
        lppt->ret = 1;
    if (lppt->lines == 5 && !strEq(line, line5))
        lppt->ret = 1;
    if (lppt->lines > 10 && lppt->lines < 99 && !strEq(line, linerepeat))
        lppt->ret = 1;
    if (lppt->lines == 99 && !strEq(line, line99))
        lppt->ret = 1;
    if (lppt->lines == 100 && !strEq(line, line100))
        lppt->ret = 1;

    return true;
}

int test_lineparse_push()
{
    StreamBuffer *sb;
    int ret = 0;
    LineParsePushTestCtx lppt = { 0 };
    string teststr_lf = 0;
    string teststr_crlf = 0;

    strCopy(&teststr_lf, (strref)testdata_lf);
    strCopy(&teststr_crlf, (strref)testdata_crlf);

    // test with a large buffer
    sb = sbufCreate(8192);
    if (!lparseRegisterPush(sb, test_linecb, test_ctxcleanup, &lppt, 0))
        return 1;
    sbufStrIn(sb, teststr_lf);

    if (lppt.lines != 100)
        ret = 1;

    sbufRelease(&sb);

    if (!lppt.didclean)
        ret = 1;
    ret |= lppt.ret;

    // test with a very small buffer
    lppt = (LineParsePushTestCtx){ 0 };

    sb = sbufCreate(5);
    if (!lparseRegisterPush(sb, test_linecb, test_ctxcleanup, &lppt, 0))
        return 1;
    sbufStrIn(sb, teststr_crlf);

    if (lppt.lines != 100)
        ret = 1;
    ret |= lppt.ret;

    sbufRelease(&sb);

    if (!lppt.didclean)
        ret = 1;

    strDestroy(&teststr_lf);
    strDestroy(&teststr_crlf);

    return ret;
}

testfunc lineparsetest_funcs[] = {
    { "explicit", test_lineparse_explicit },
    { "auto", test_lineparse_auto },
    { "mixed", test_lineparse_mixed },
    { "push", test_lineparse_push },
    { 0, 0 }
};
