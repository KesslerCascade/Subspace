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

int64_u _int64_add(int64_u a, int64_u b)
{
    int64_u result;

    result.p.low = a.p.low + b.p.low;

    // Carry if overflow in low 32 bits
    uint32_t carry = (result.p.low < a.p.low) ? 1 : 0;

    result.p.high = a.p.high + b.p.high + carry;

    return result;
}
