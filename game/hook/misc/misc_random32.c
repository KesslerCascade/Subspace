#include "feature/feature.h"
#include "feature/tweaks.h"
#include "ftl/misc.h"
#include "hook/hook.h"
#include "patch/patchlist.h"
#include "int64.h"
#include "pcg.h"

static pcg_state temp;

// ---- Hooks ----------------

int32_t subspace_random32()
{
    return random32();
    // return (int32_t)(pcg_random(&temp) & 0x7fffffff);
}

void subspace_srandom32(int32_t seed)
{
    srandom32(seed);
    // pcg_seed(&temp, seed, 0);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, random32, subspace_random32) &&
        replaceFunction(base, srandom32, subspace_srandom32);
}

Patch patch_Misc_Random32 = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(random32), &SYM(srandom32), 0 }
};
