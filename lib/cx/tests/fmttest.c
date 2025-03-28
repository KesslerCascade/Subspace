#include <cx/container.h>
#include <cx/format.h>
#include <cx/string.h>

#include <math.h>

#include "fmttestobj.h"

#define TEST_FILE fmttest
#define TEST_FUNCS fmttest_funcs
#include "common.h"

static int test_string()
{
    string res = 0;
    string in1 = 0, in2 = 0, in3 = 0;

    strNConcat(&in1, _S"Te", _S"st1");
    strNConcat(&in2, _S"Te", _S"st2");
    strNConcat(&in3, _S"Te", _S"st3");

    strFormat(&res, _S"This is a ${string}, ${string}, ${string}",
              stvar(string, in1), stvar(string, in2), stvar(string, in3));

    if (!strEq(res, _S"This is a Test1, Test2, Test3"))
        return 1;

    strFormat(&res, _S"This is a ${string2}, ${string3}, ${string1}",
              stvar(string, in1), stvar(string, in2), stvar(string, in3));

    if (!strEq(res, _S"This is a Test2, Test3, Test1"))
        return 1;

    strFormat(&res, _S"This is a ${string2}, ${string1}, ${string}",
              stvar(string, in1), stvar(string, in2), stvar(string, in3));

    if (!strEq(res, _S"This is a Test2, Test1, Test1"))
        return 1;

    strFormat(&res, _S"${string1}:${string2(8,upper)}:${string3(8,right,lower)}:${string1(center,9)}",
              stvar(string, in1), stvar(string, in2), stvar(string, in3));

    if (!strEq(res, _S"Test1:TEST2   :   test3:  Test1  "))
        return 1;

    strDestroy(&in1);
    strDestroy(&in2);
    strDestroy(&in3);
    strDestroy(&res);

    return 0;
}

static int test_int()
{
    string res = 0;

    strFormat(&res, _S"This is a ${int}, ${uint}, ${int}, ${uint}, ${int}, ${uint} test",
              stvar(int8, 17), stvar(int16, -23102), stvar(int32, 1000000001),
              stvar(uint16, 65535), stvar(uint64, 0xffffffffffffffff), stvar(uint32, 0));

    if (!strEq(res, _S"This is a 17, 65535, -23102, 18446744073709551615, 1000000001, 0 test"))
        return 1;

    strFormat(&res, _S"This is a ${int(7)}:${int(7,left)}:${0int(7)}:${0int(7)}:${-int}:${-int}:${+int(7)}:${int(min:7)}:${int(min:7)} test",
              stvar(int32, 17), stvar(int32, -17), stvar(int32, 17), stvar(int32, -17), stvar(int32, 17), stvar(int32, -17),
              stvar(int32, 17), stvar(int32, 17), stvar(int32, -17));

    if (!strEq(res, _S"This is a      17:-17    :0000017:-000017: 17:-17:    +17:0000017:-0000017 test"))
        return 1;

    strFormat(&res, _S"This is a ${uint(hex)}, ${uint(octal)}, ${uint(binary)}, ${uint(octal,prefix)}, ${uint(prefix,upper,hex)} test",
              stvar(uint32, 0xbeeff00d), stvar(uint32, 0377), stvar(uint32, 273), stvar(uint32, 0755), stvar(uint32, 0xd00daf0e));

    if (!strEq(res, _S"This is a beeff00d, 377, 100010001, 0755, 0xD00DAF0E test"))
        return 1;

    strFormat(&res, _S"This is a ${int(utfchar)}${int(utfchar)}${int(utfchar)} test",
              stvar(int32, 0x306f), stvar(int32, 0x3067), stvar(int32, 0x306a));

    if (!strEq(res, _SU"This is a はでな test"))
        return 1;

    strDestroy(&res);

    return 0;
}

static int test_float()
{
    string res = 0;

    strFormat(&res, _S"This is a ${float}, ${float}, ${float}, ${float}, ${float}, ${float} test",
              stvar(float32, 1.75f), stvar(float64, -1728.023), stvar(float64, 10.83e+22),
              stvar(float32, 65535.f), stvar(float64, 3.1415e-11), stvar(float64, -4.455e-50));

    if (!strEq(res, _S"This is a 1.75, -1728.023, 1.083e+23, 65535, 3.1415e-11, -4.455e-50 test"))
        return 1;

    strFormat(&res, _S"This is a ${float}, ${float}, ${float} test",
              stvar(float32, NAN), stvar(float32, INFINITY), stvar(float64, -INFINITY));

    if (!strEq(res, _S"This is a NaN, Inf, -Inf test"))
        return 1;

    strFormat(&res, _S"This is a ${float(sig:4)}, ${float(sig:4)}, ${float(dec:4)}, ${float(dec:4,zero)}, ${float(dec:4,zero)} test",
              stvar(float64, -173.2134), stvar(float64, 1.67988e+18), stvar(float64, 5.48108), stvar(float64, 5.479969), stvar(float64, -1.1));

    if (!strEq(res, _S"This is a -173.2, 1.68e+18, 5.4811, 5.4800, -1.1000 test"))
        return 1;

    strFormat(&res, _S"This is a ${float(fixed)}, ${float(fixed,zero)}, ${float(fixed,dec:3)}, ${float(fixed,dec:3)}, ${float(fixed,dec:30)} test",
              stvar(float64, 0.0043834582), stvar(float64, 1732000), stvar(float64, 1.7834e+22), stvar(float64, 1.7834e-22), stvar(float64, 1.7834e-22));

    if (!strEq(res, _S"This is a 0.004383, 1732000.000000, 17834000000000000000000, 0, 0.00000000000000000000017834 test"))
        return 1;

    strFormat(&res, _S"This is a ${0float1(10)}, ${0float1(7)}, ${0float1(5)}, ${0float1(4)} test",
              stvar(float64, -1.17384));

    if (!strEq(res, _S"This is a -001.17384, -1.1738, -1.17, -1.2 test"))
        return 1;

    strDestroy(&res);
    return 0;
}

static int test_ptr()
{
    string res = 0;
    void *testptr = (void*)0x1234dead;

    strFormat(&res, _S"This is a ${ptr1(upper)}, ${ptr1(prefix)} test", stvar(ptr, testptr));

    if (!strEq(res, _S"This is a 1234DEAD, 0x1234dead test"))
        return 1;

    testptr = (void*)0x45babe;
    strFormat(&res, _S"This is a ${0ptr(prefix)} test", stvar(ptr, testptr));
#ifdef _64BIT
    if (!strEq(res, _S"This is a 0x000000000045babe test"))
        return 1;
#else
    if (!strEq(res, _S"This is a 0x0045babe test"))
        return 1;
#endif

    strDestroy(&res);
    return 0;
}

static int test_suid()
{
    string res = 0;

    SUID testsuid;
    // not a real SUID, just a test pattern
    testsuid.high = 0x123456789abcdef0;
    testsuid.low = 1;

    strFormat(&res, _S"This is a ${suid} test", stvar(suid, testsuid));

    if (!strEq(res, _S"This is a 0j6hb7h6nwvvr0000000000001 test"))
        return 1;

    strDestroy(&res);
    return 0;
}

static int test_object()
{
    string res = 0;

    FmtTestClass *o1, *o2, *o3, *o4, *o5;

    o1 = fmttestclassCreate(1, _S"Test");
    o2 = fmttestclassCreate(2, _S"Lest");
    o3 = fmttestclassCreate(3, _S"Best");
    o4 = fmttestclassCreate(4, _S"Fest");
    o5 = fmttestclassCreate(5, _S"Behest");

    strFormat(&res, _S"This is a ${object}, ${object}, ${object}, ${object}, ${object} test",
              stvar(object, o1), stvar(object, o2), stvar(object, o3), stvar(object, o4), stvar(object, o5));

    if (!strEq(res, _S"This is a Object(Test:One), Object(Lest:Two), Object(Best:Three), Object(Fest:Four), Object(Behest:Five) test"))
        return 1;

    o1->iv = 5;
    o2->iv = 4;
    o4->iv = 2;
    o5->iv = 1;

    strFormat(&res, _S"This is a ${object}, ${object}, ${object}, ${object}, ${object} test",
              stvar(object, o1), stvar(object, o2), stvar(object, o3), stvar(object, o4), stvar(object, o5));

    if (!strEq(res, _S"This is a Object(Test:Five), Object(Lest:Four), Object(Best:Three), Object(Fest:Two), Object(Behest:One) test"))
        return 1;

    objRelease(&o1);
    objRelease(&o2);
    objRelease(&o3);
    objRelease(&o4);
    objRelease(&o5);
    strDestroy(&res);

    // try it again but using a class that implements Convertible instead of Formattable
    FmtTestClass2 *oo1, *oo2, *oo3, *oo4, *oo5;

    oo1 = fmttestclass2Create(1, _S"Test");
    oo2 = fmttestclass2Create(2, _S"Lest");
    oo3 = fmttestclass2Create(3, _S"Best");
    oo4 = fmttestclass2Create(4, _S"Fest");
    oo5 = fmttestclass2Create(5, _S"Behest");

    strFormat(&res, _S"This is a ${object}, ${object}, ${object}, ${object}, ${object} test",
              stvar(object, oo1), stvar(object, oo2), stvar(object, oo3), stvar(object, oo4), stvar(object, oo5));

    if (!strEq(res, _S"This is a Object(Test:One), Object(Lest:Two), Object(Best:Three), Object(Fest:Four), Object(Behest:Five) test"))
        return 1;

    oo1->iv = 5;
    oo2->iv = 4;
    oo4->iv = 2;
    oo5->iv = 1;

    strFormat(&res, _S"This is a ${object}, ${object}, ${object}, ${object}, ${object} test",
              stvar(object, oo1), stvar(object, oo2), stvar(object, oo3), stvar(object, oo4), stvar(object, oo5));

    if (!strEq(res, _S"This is a Object(Test:Five), Object(Lest:Four), Object(Best:Three), Object(Fest:Two), Object(Behest:One) test"))
        return 1;

    objRelease(&oo1);
    objRelease(&oo2);
    objRelease(&oo3);
    objRelease(&oo4);
    objRelease(&oo5);
    strDestroy(&res);

    return 0;
}

static int test_array()
{
    string res = 0;

    sa_int32 intarray;
    saInit(&intarray, int32, 5);
    sa_string strarray;
    saInit(&strarray, string, 5);

    saPush(&intarray, int32, 32);
    saPush(&intarray, int32, 33);
    saPush(&intarray, int32, 34);
    saPush(&intarray, int32, 35);
    saPush(&intarray, int32, 36);

    saPush(&strarray, string, _S"Test");
    saPush(&strarray, string, _S"Of");
    saPush(&strarray, string, _S"Array");
    saPush(&strarray, string, _S"Formatting");
    saPush(&strarray, string, _S"Awesome");

    strFormat(&res, _S"This is a ${int[4]}, ${int[1]}, ${int[3]}, ${int[0]}, ${int[2]} test",
              stvar(sarray, intarray));

    if (!strEq(res, _S"This is a 36, 33, 35, 32, 34 test"))
        return 1;

    strFormat(&res, _S"This is a ${int[]}, ${int[]}, ${int[]}, ${int[]}, ${int[]} test",
              stvar(sarray, intarray));

    if (!strEq(res, _S"This is a 32, 33, 34, 35, 36 test"))
        return 1;

    strFormat(&res, _S"This is a ${string[0]} ${string[1]} ${string[4]} ${string[2]} ${string[3]}",
              stvar(sarray, strarray));

    if (!strEq(res, _S"This is a Test Of Awesome Array Formatting"))
        return 1;

    strFormat(&res, _S"This is an ${string[4]} ${string[0]} ${string[1]} ${string[0]} ${string[3]}",
              stvar(sarray, intarray), stvar(sarray, strarray));

    if (!strEq(res, _S"This is an Awesome Test Of Test Formatting"))
        return 1;

    saDestroy(&intarray);
    saDestroy(&strarray);

    strDestroy(&res);
    return 0;
}

static int test_hash()
{
    string res = 0;
    hashtable testht;
    htInit(&testht, string, float64, 8);

    htInsert(&testht, string, _S"one", float64, 1);
    htInsert(&testht, string, _S"sqrttwo", float64, 1.41421);
    htInsert(&testht, string, _S"two", float64, 2);
    htInsert(&testht, string, _S"e", float64, 2.71828);
    htInsert(&testht, string, _S"three", float64, 3);
    htInsert(&testht, string, _S"pi", float64, 3.14159);
    htInsert(&testht, string, _S"four", float64, 4);

    strFormat(&res, _S"It's easy as ${float:one}, ${float:two}, ${float:three}",
              stvar(hashtable, testht));

    if (!strEq(res, _S"It's easy as 1, 2, 3"))
        return 1;

    strFormat(&res, _S"It's easy as ${float:four}, ${float:pi}, ${float:e}?",
              stvar(hashtable, testht));

    if (!strEq(res, _S"It's easy as 4, 3.14159, 2.71828?"))
        return 1;

    strFormat(&res, _S"sqrt(${float:two}) = ${float:sqrttwo}",
              stvar(hashtable, testht));

    if (!strEq(res, _S"sqrt(2) = 1.41421"))
        return 1;

    htDestroy(&testht);
    strDestroy(&res);
    return 0;
}

static int test_default()
{
    string res = 0;

    strFormat(&res, _S"This is a ${int;55} ${string;unused} ${string;default} ${int;0} test",
              stvar(int32, 1702), stvar(string, _S"specified"));

    if (!strEq(res, _S"This is a 1702 specified default 0 test"))
        return 1;

    strDestroy(&res);
    return 0;
}

static int test_error()
{
    string res = 0;

    strDup(&res, _S"canary");

    if (strFormat(&res, _S"This ${int} ${int} ${int} test should fail",
        stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    if (strFormat(&res, _S"This ${int[0]} test should fail",
                  stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    if (strFormat(&res, _S"This ${string} test should fail",
                  stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    if (strFormat(&res, _S"This ${int:hash} test should fail",
                  stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    if (strFormat(&res, _S"This ${int test should fail",
                  stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    if (strFormat(&res, _S"This ${int(asdf} test should fail",
                  stvar(int32, 5), stvar(int32, 10)) != false)
        return 1;
    if (!strEmpty(res))
        return 1;

    strDestroy(&res);
    return 0;
}

testfunc fmttest_funcs[] = {
    { "int", test_int },
    { "float", test_float },
    { "string", test_string },
    { "ptr", test_ptr },
    { "suid", test_suid },
    { "object", test_object },
    { "array", test_array },
    { "hash", test_hash },
    { "default", test_default },
    { "error", test_error },
    { 0, 0 }
};
