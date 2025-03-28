#include <stdio.h>
#include <cx/math.h>
#include <cx/string.h>

#define TEST_FILE mathtest
#define TEST_FUNCS mathtest_funcs
#include "common.h"

static int test_math_pcgint()
{
    PcgState rng;
    pcgAutoSeed(&rng);
    uint32 r;
    int32 sr;
    uint64 v;
    int64 sv;

    // generate a bunch of random numbers and verify they fall in the expected range

    for (int loop = 0; loop < 10000; loop++) {
        for (uint32 i = 0; i < 2147483647; i += (i >> 2) + 1) {
            r = pcgBounded(&rng, i);
            if (r >= i && i != 0)
                return 1;
        }

        for (uint32 i = 0; i < 2147483647; i += (i >> 2) + 1) {
            r = pcgRange(&rng, (i >> 2), i);
            if (r < (i >> 2) || r > i)
                return 1;
        }

        for (int32 i = 0; i < 1073741823; i += (i >> 2) + 1) {
            sr = pcgSRange(&rng, -i, i);
            if (sr < -i || sr > i)
                return 1;
        }

        for (uint64 i = 0; i < 9223372036854775807ULL; i += (i >> 2) + 1) {
            v = pcgBounded64(&rng, i);
            if (v >= i && v != 0)
                return 1;
        }

        for (uint64 i = 0; i < 9223372036854775807ULL; i += (i >> 2) + 1) {
            v = pcgRange64(&rng, (i >> 2), i);
            if (v < (i >> 2) || v > i)
                return 1;
        }

        for (int64 i = 0; i < 4611686018427387903LL; i += (i >> 2) + 1) {
            sv = pcgSRange64(&rng, (i >> 2), i);
            if (sv < (i >> 2) || sv > i)
                return 1;
        }
    }

    // check that range actually returns everything
    // count should never reach anywhere remotely close to 10000,
    // it's an extreme limit to keep it from running forever if generation is broken

    uint32 seen = 0;
    int count = 0;

    for (count = 0; seen != 0x3ff && count < 10000; count++) {
        r = pcgBounded(&rng, 10);
        if (r >= 10)
            return 1;

        seen |= 1 << r;
    }

    if (seen != 0x3ff)
        return 1;

    seen = 0;

    for (count = 0; seen != 0x3ff && count < 10000; count++) {
        r = pcgRange(&rng, 31, 40);
        if (r < 31 || r > 40)
            return 1;

        seen |= 1 << (r-31);
    }

    if (seen != 0x3ff)
        return 1;

    return 0;
}

static int test_math_pcgfloat()
{
    PcgState rng;
    pcgAutoSeed(&rng);

    float32 r;
    float64 v;

    for (int loop = 0; loop < 10000; loop++) {
        for (float32 i = 0; i < 1e+20; i += i/2 + 1) {
            r = pcgFRange(&rng, -i, i);
            if (r < -i || r > i)
                return 1;
        }

        //NOLINTNEXTLINE
        for (float64 i = 0; i < 1e+40; i += i/2 + 1) {
            v = pcgFRange64(&rng, -i, i);
            if (v < -i || v > i)
                return 1;
        }
    }

    // check floating point range
    uint32 seen = 0;
    int count = 0;
    uint32 bit = 0;

    for (count = 0; seen != 0xffffffff && count < 100000; count++) {
        r = pcgFRange(&rng, 7, 10.1f);
        bit = (uint32)((r - 7) * 10 + 0.5);
        if (bit > 31)
            return 1;

        seen |= (1 << bit);
    }

    if (seen != 0xffffffff)
        return 1;
    seen = 0;

    for (count = 0; seen != 0xffffffff && count < 100000; count++) {
        v = pcgFRange64(&rng, -18.1, -15);
        bit = (uint32)((-v - 15) * 10 + 0.5);
        if (bit > 31)
            return 1;

        seen |= (1 << bit);
    }

    if (seen != 0xffffffff)
        return 1;

    return 0;
}

static int test_math_pcgerror()
{
    PcgState rng;
    pcgAutoSeed(&rng);

    // try a lot of things that should not work and verify they return
    // expected results

    if (pcgBounded(&rng, 0) != 0)
        return 1;

    if (pcgBounded64(&rng, 0) != 0)
        return 1;

    if (pcgRange(&rng, 5, 5) != 5)
        return 1;

    if (pcgRange(&rng, 9, 5) != 9)
        return 1;

    if (pcgRange64(&rng, 9000000000000LL, 5000000000000LL) != 9000000000000LL)
        return 1;

    if (pcgSRange(&rng, -50, -90) != -50)
        return 1;

    if (pcgFRange(&rng, 401, 5) != 401)
        return 1;

    if (pcgFRange64(&rng, -4029413, -9999999999) != -4029413)
        return 1;

    return 0;
}

static int test_math_floatcmp()
{
    // these two numbers are only 1 ULP apart
    float32 float1 = 80087352;
    float32 float2 = 80087360;

    // IEEE-754 sanity check
    if (float1 == float2)
        return 1;

    if (stCmp(float32, float1, float2) != 0)
        return 1;

    float64 float3 = 0.865887489;
    float64 float4 = 8.65887489000000121208699965791E-1;
    float64 float5 = 0.865887488;
    float64 float6 = 0.865887490;

    if (float1 == float2)
        return 1;

    if (stCmp(float64, float3, float4) != 0)
        return 1;

    if (stCmp(float64, float3, float5) != 1)
        return 1;

    if (stCmp(float64, float3, float6) != -1)
        return 1;

    float32 fz1 = 0.0;
    float32 fz2 = -0.0;
    float32 fnz1 = 1.40129846432e-45f;
    float32 fnz2 = -1.40129846432e-45f;

    // 0 should equal -0
    if (stCmp(float32, fz1, fz2) != 0)
        return 1;

    // these are close enough to zero they should be considered equivalent
    if (stCmp(float32, fz1, fnz1) != 0)
        return 1;
    if (stCmp(float32, fz2, fnz2) != 0)
        return 1;

    // but differ by sign even though they're within the threshold
    if (stCmp(float32, fnz1, fnz2) != 1)
        return 1;
    if (stCmp(float32, fnz2, fnz1) != -1)
        return 1;

    if (stCmp(float32, fz2, fnz1) != -1)
        return 1;
    if (stCmp(float32, fz1, fnz2) != 1)
        return 1;

    return 0;
}

testfunc mathtest_funcs[] = {
    { "pcgint", test_math_pcgint },
    { "pcgfloat", test_math_pcgfloat },
    { "pcgerror", test_math_pcgerror },
    { "floatcmp", test_math_floatcmp },
    { 0, 0 }
};
