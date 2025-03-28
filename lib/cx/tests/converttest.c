#include <stdio.h>
#include <cx/string.h>
#include <cx/stype/stvar.h>
#include <cx/string/strtest.h>
#include "converttestobj.h"

#define TEST_FILE converttest
#define TEST_FUNCS converttest_functions
#include "common.h"

static int test_numeric()
{
    int ret = 0;

    int8 tiny;
    int16 med;
    int32 big;
    int64 huge;

    uint8 byte;
    uint16 word;
    uint32 dword;
    uint64 qword;

    float32 singy;
    float64 doublet;

    if (!stConvert(int32, &big, int16, 7) || big != 7)
        ret = 1;

    if (!stConvert(int16, &med, uint64, 102) || med != 102)
        ret = 1;

    if (!stConvert(uint8, &byte, int32, 77) || byte != 77)
        ret = 1;

    if (!stConvert(float64, &doublet, uint16, 1023) || doublet != 1023)
        ret = 1;

    if (!stConvert(uint64, &qword, float32, 9) || qword != 9)
        ret = 1;

    if (!stConvert(int64, &huge, uint64, 0x7fffffffffffffffLL) || huge != 0x7fffffffffffffffLL)
        ret = 1;

    if (!stConvert(uint16, &word, int32, 65535) || word != 65535)
        ret = 1;

    // these should all fail
    if (stConvert(uint32, &dword, int16, -5))
        ret = 1;

    if (stConvert(int8, &tiny, uint32, 99999))
        ret = 1;

    if (stConvert(int16, &med, uint16, 32769))
        ret = 1;

    if (stConvert(float32, &singy, int64, 25000000, ST_Lossless))
        ret = 1;

    // now try them again with overflow set
    if (!stConvert(uint32, &dword, int16, -5, ST_Overflow) || dword != 0xfffffffb)
        ret = 1;

    if (!stConvert(int8, &tiny, uint32, 99999, ST_Overflow) || tiny != -97)
        ret = 1;

    if (!stConvert(int16, &med, uint16, 32769, ST_Overflow) || med != -32767)
        ret = 1;

    // without ST_Lossless should round to nearest float
    if (!stConvert(float32, &singy, int64, 25000001) || singy != 25000000)
        ret = 1;

    // dumb conversion but it should still work
    stvar sv;
    if (!stConvert(stvar, &sv, uint32, 0xfffe1011) ||
        stGetId(sv.type) != stTypeId(uint32) ||
        sv.data.st_uint32 != 0xfffe1011)
        ret = 1;

    if (!stConvert(int64, &huge, stvar, sv) || huge != 0xfffe1011)
        ret = 1;

    return ret;
}

static int test_string()
{
    int ret = 0;
    string test1 = 0;
    string test2 = 0;
    int64 i64;
    uint32 u32;
    uint16 u16;
    int8 i8;

    if (!stConvert(string, &test1, int32, 10754) || !strEq(test1, _S"10754"))
        ret = 1;
    strDestroy(&test1);

    if (!stConvert(string, &test1, float64, 22901.4434) || !strEq(test1, _S"22901.4434"))
        ret = 1;
    strDestroy(&test1);

    if (!stConvert(string, &test1, stvar, stvar(int16, -301)) || !strEq(test1, _S"-301"))
        ret = 1;
    strDestroy(&test1);

    if (!stConvert(int64, &i64, string, _S"203941") || i64 != 203941)
        ret = 1;

    if (!stConvert(uint32, &u32, string, _S"0x90102034") || u32 != 0x90102034)
        ret = 1;

    if (!stConvert(uint16, &u16, string, _S"65535") || u16 != 65535)
        ret = 1;

    if (stConvert(int8, &i8, string, _S"128"))
        ret = 1;

    if (!stConvert(int8, &i8, string, _S"128", ST_Overflow) || i8 != -128)
        ret = 1;

    SUID tsuid;
    tsuid.high = 1;
    tsuid.low = 0;
    if (!stConvert(string, &test1, suid, tsuid) || !strEq(test1, _S"0000000000000g000000000000"))
        ret = 1;
    strDestroy(&test1);

    if (!stConvert(suid, &tsuid, string, _S"00000000000000000000000005") || tsuid.high != 0 || tsuid.low != 5)
        ret = 1;

    // converting a string to a string should just copy it
    strCopy(&test1, _S"Test String");
    if (!stConvert(string, &test2, string, test1) || !strEq(test1, test2) ||
        strTestRefCount(test1) != 2 || strTestRefCount(test2) != 2)
        ret = 1;

    strDestroy(&test1);
    strDestroy(&test2);

    return ret;
}

int test_object()
{
    int ret = 0;

    ConvertTestClass *ctc = converttestclassCreate(17, 220.43, _S"Object Test 1");

    int16 small;
    float32 shorty;
    string strtest = 0;

    if (!stConvert(int16, &small, object, ctc) || small != 17)
        ret = 1;

    if (!stConvert(float32, &shorty, object, ctc) || shorty != 220.43f)
        ret = 1;

    if (!stConvert(string, &strtest, object, ctc) || !strEq(strtest, _S"Object Test 1"))
        ret = 1;

    objRelease(&ctc);
    strDestroy(&strtest);

    return ret;
}

testfunc converttest_functions[] = {
    { "numeric", test_numeric },
    { "string", test_string },
    { "object", test_object },
    { 0, 0 }
};
