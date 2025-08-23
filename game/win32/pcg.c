#include "pcg.h"
#include "int64.h"

// This file lives on OS dependant code because it needs special handling on Win32 to do 64-bit math

void pcg_seed(pcg_state* rng, uint64_t initstate, uint64_t initseq)
{
    rng->state = 0U;
    rng->inc   = uint64_shl(initseq, 1) | 1u;
    pcg_random(rng);
    rng->state = uint64_add(rng->state, initstate);
    pcg_random(rng);
}

uint32_t pcg_random(pcg_state* rng)
{
    uint64_t oldstate = rng->state;
    rng->state = uint64_add(uint64_mul_lo(oldstate, 6364136223846793005ULL), (rng->inc | 1u));
    uint32_t xorshifted = (uint32_t)uint64_shr(uint64_shr(oldstate, 18) ^ oldstate, 27);
    uint32_t rot        = (uint32_t)uint64_shr(oldstate, 59);
    return (xorshifted >> rot) | (xorshifted << ((1 + ~rot) & 31));
}
