#pragma once

// Somewhat modified version of PCG, coverd by APL below:
// Extremely fast pseudo RNG with good statistical properties.

/*
* PCG Random Number Generation for C.
*
* Copyright 2014 Melissa O'Neill <oneill@pcg-random.org>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*/

#include <cx/cx.h>
#include <cx/debug/assert.h>

CX_C_BEGIN

typedef struct PcgState {   // Internals are *Private*.
    uint64 state;           // RNG state.  All values are possible.
    uint64 inc;             // Controls which RNG sequence (stream) is
                            // selected. Must *always* be odd.
} PcgState;

// Seeds the random number generator with an initial state and sequence (stream)
void pcgSeed(_Out_ PcgState* rng, uint64 initstate, uint64 initseq);

// Seeds the random number generator using cryptographic-quality entropy.
// Does not permit stream selection as it's meaningless when not starting
// from a known state.
void pcgAutoSeed(_Out_ PcgState* rng);

// Generates a random integer from [0..UINT32_MAX]
uint32 pcgRandom(_Inout_ PcgState *rng);

// Generates a random integer from [0..bound)
// unlike pcgRandom() % bound, does not suffer from bias
uint32 pcgBounded(_Inout_ PcgState *rng, uint32 bound);

// Simulates a coin flip, returns true or false
bool pcgFlip(_Inout_ PcgState *rng);

// Generates a random integer from [0..bound)
// unlike pcgRandom() % bound, does not suffer from bias
_meta_inline int32 pcgSBounded(_Inout_ PcgState *rng, int32 bound)
{
    devAssert(bound >= 0);
    return (int32)pcgBounded(rng, bound);
}

// Generates a random integer from [lower..upper]
_meta_inline uint32 pcgRange(_Inout_ PcgState *rng, uint32 lower, uint32 upper)
{
    if (lower >= upper)
        return lower;

    return lower + pcgBounded(rng, upper - lower + 1);
}

// Generates a random integer from [lower..upper]
_meta_inline int32 pcgSRange(_Inout_ PcgState *rng, int32 lower, int32 upper)
{
    if (lower >= upper)
        return lower;

    return (int32)(lower + pcgBounded(rng, (uint32)(upper - lower + 1)));
}

// Generates a random integer from [0..UINT64_MAX]
uint64 pcgRandom64(_Inout_ PcgState *rng);

// Generates a random integer from [0..bound)
// unlike pcgRandom() % bound, does not suffer from bias
uint64 pcgBounded64(_Inout_ PcgState *rng, uint64 bound);

// Generates a random integer from [lower..upper]
_meta_inline uint64 pcgRange64(_Inout_ PcgState *rng, uint64 lower, uint64 upper)
{
    if (lower >= upper)
        return lower;

    return lower + pcgBounded64(rng, upper - lower + 1);
}

// Generates a random integer from [lower..upper]
_meta_inline int64 pcgSRange64(_Inout_ PcgState *rng, int64 lower, int64 upper)
{
    if (lower >= upper)
        return lower;

    return (int64)(lower + pcgBounded64(rng, (uint64)(upper - lower + 1)));
}

// Generates a random floating point number from [lower..upper]
float32 pcgFRange(_Inout_ PcgState *rng, float32 lower, float32 upper);

// Generates a random floating point number from [lower..upper]
float64 pcgFRange64(_Inout_ PcgState *rng, float64 lower, float64 upper);

// Advances the RNG state by delta iterations
void pcgAdvance(_Inout_ PcgState *rng, uint64 delta);

CX_C_END
