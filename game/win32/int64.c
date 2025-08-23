#include "int64.h"
#include <stdint.h>

// Based on __div64_32 from linux kernel
uint64_u _uint64_div(uint64_u dividend, uint32_t divisor, uint32_t* remainder)
{
    uint64_u rem = dividend;
    uint64_u b   = { .v = divisor };
    uint64_u res = { .p.high = dividend.p.high }, d = { .v = 1 };

    /* Reduce the thing a bit first */
    if (res.p.high >= divisor) {
        res.p.high /= divisor;
        rem.p.high -= (res.p.high * divisor);
    }

    while ((int64_t)b.v > 0 && b.v < rem.v) {
        b = _uint64_add(b, b);
        d = _uint64_add(d, d);
    }

    do {
        if (rem.v >= b.v) {
            rem = _uint64_sub(rem, b);
            res = _uint64_add(res, d);
        }
        b.p.low >>= 1;
        b.p.low |= (b.p.high & 1) << 31;
        b.p.high >>= 1;
        d.p.low >>= 1;
        d.p.low |= (d.p.high & 1) << 31;
        d.p.high >>= 1;
    } while (d.v);

    if (remainder)
        *remainder = (uint32_t)rem.v;
    return res;
}

static uint64_u _uint32_mul(uint32_t a, uint32_t b)
{
    uint32_t p00, p11, p10, p01;
    p11 = (a >> 16) * (b >> 16);
    p01 = (a & 0xFFFF) * (b >> 16);
    p10 = (a >> 16) * (b & 0xFFFF);
    p00 = (a & 0xFFFF) * (b & 0xFFFF);

    uint32_t middle = p10 + (p00 >> 16) + (p01 & 0xFFFF);
    uint64_u ret    = { 0 };
    ret.p.high      = p11 + (middle >> 16) + (p01 >> 16);
    ret.p.low       = (middle << 16) | (p00 & 0xFFFF);
    return ret;
}

uint64_u _uint64_mul(uint64_u* high, uint64_u a, uint64_u b)
{
    // algorithm borrowed from https://github.com/catid/fp61
    uint64_u p00, p11, p10, p01;
    p11 = _uint32_mul(a.p.high, b.p.high);
    p01 = _uint32_mul(a.p.low, b.p.high);
    p10 = _uint32_mul(a.p.high, b.p.low);
    p00 = _uint32_mul(a.p.low, b.p.low);

    // carry
    uint64_u t1     = { .p.low = p00.p.high };
    uint64_u t2     = { .p.low = p01.p.low };
    uint64_u t3     = _uint64_add(t1, t2);
    uint64_u middle = _uint64_add(p10, t3);

    if (high) {
        t1.p.low  = middle.p.high;
        t1.p.high = 0;
        t2.p.low  = p01.p.high;
        t2.p.high = 0;
        t3        = _uint64_add(t1, t2);
        *high     = _uint64_add(p11, t3);
    }

    uint64_u ret;
    ret.p.high = middle.p.low;
    ret.p.low  = p00.p.low;
    return ret;
}

typedef struct {
    uint32_t low;
    uint32_t high;
} uint64_s;

uint64_u _uint64_add(uint64_u a, uint64_u b)
{
    uint64_u result;

    result.p.low = a.p.low + b.p.low;

    // Carry if overflow in low 32 bits
    uint32_t carry = (result.p.low < a.p.low) ? 1 : 0;

    result.p.high = a.p.high + b.p.high + carry;

    return result;
}

uint64_u _uint64_sub(uint64_u a, uint64_u b)
{
    uint64_u result;

    // calculate two's complement of b
    b.p.low        = ~b.p.low;
    b.p.high       = ~b.p.high;
    b              = _uint64_add(b, (uint64_u) { .v = 1 });

    result.p.low = a.p.low + b.p.low;

    // Carry if overflow in low 32 bits
    uint32_t carry = (result.p.low < a.p.low) ? 1 : 0;

    result.p.high = a.p.high + b.p.high + carry;

    return result;
}

uint64_u _uint64_shl(uint64_u a, int b)
{
    if (b <= 0)
        return a;

    uint64_u result;
    if (b < 32) {
        result.p.high = a.p.high << b;
        result.p.high |= a.p.low >> (32 - b);
        result.p.low = a.p.low << b;
    } else if (b < 64) {
        result.p.low  = 0;
        result.p.high = a.p.low << (b - 32);
    } else {
        result.v = 0;
    }
    return result;
}

uint64_u _uint64_shr(uint64_u a, int b)
{
    if (b <= 0)
        return a;

    uint64_u result;
    if (b < 32) {
        result.p.low = a.p.low >> b;
        if (b > 0 && b < 32)
            result.p.low |= a.p.high << (32 - b);
        result.p.high = a.p.high >> b;
    } else if (b < 64) {
        result.p.high = 0;
        result.p.low  = a.p.high >> (b - 32);
    } else {
        result.v = 0;
    }
    return result;
}

int64_u _int64_add(int64_u a, int64_u b)
{
    int64_u result;

    result.p.low = a.p.low + b.p.low;

    // Carry if overflow in low 32 bits
    uint32_t carry = (result.p.low < a.p.low) ? 1 : 0;

    result.p.high = a.p.high + b.p.high + carry;

    return result;
}
