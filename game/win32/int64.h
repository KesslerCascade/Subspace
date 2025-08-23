#include <stdint.h>

// Helper functions for doing 64-bit arithmetic on 32-bit CPUs *without* library support for
// functions like __udivdi3 or similar

typedef union int64_u {
    int64_t v;
    struct {
        uint32_t low;
        int32_t high;
    } p;
} int64_u;

typedef union uint64_u {
    uint64_t v;
    struct {
        uint32_t low;
        uint32_t high;
    } p;
} uint64_u;

uint64_u _uint64_div(uint64_u dividend, uint32_t divisor, uint32_t *remainder);
uint64_u _uint64_mul(uint64_u* high, uint64_u a, uint64_u b);
uint64_u _uint64_add(uint64_u a, uint64_u b);
uint64_u _uint64_sub(uint64_u a, uint64_u b);
uint64_u _uint64_shl(uint64_u a, int b);
uint64_u _uint64_shr(uint64_u a, int b);
int64_u _int64_add(int64_u a, int64_u b);

static uint64_t uint64_div(uint64_t dividend, uint32_t divisor, uint32_t *remainder) {
    return _uint64_div(*(uint64_u*)&dividend, divisor, remainder).v;
}

static uint64_t uint64_mul_lo(uint64_t a, uint64_t b)
{
    return _uint64_mul(NULL, *(uint64_u*)&a, *(uint64_u*)&b).v;
}

static uint64_t uint64_add(uint64_t a, uint64_t b) {
    return _uint64_add(*(uint64_u*)&a, *(uint64_u*)&b).v;
}

static uint64_t uint64_shl(uint64_t a, int b)
{
    return _uint64_shl(*(uint64_u*)&a, b).v;
}

static uint64_t uint64_shr(uint64_t a, int b)
{
    return _uint64_shr(*(uint64_u*)&a, b).v;
}

static int64_t int64_add(int64_t a, int64_t b) {
    return _int64_add(*(int64_u*)&a, *(int64_u*)&b).v;
}