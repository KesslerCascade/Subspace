#include <cx/cx.h>

// Basic linear congruential PRNG.
// Not very good quality output, but simple, fast, inline, and only
// needs 32 bits of state.

// Use PCG for better quality with very good performance.

#define LCG_MAX (0x7ffffffe)

_meta_inline int32 lcgRandom(_Inout_ uint32 *state)
{
    return ((*state = *state * 1103515245 + 12345) % ((uint32)LCG_MAX + 1));
}
