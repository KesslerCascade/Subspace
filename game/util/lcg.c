#include "ftl/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include "lcg.h"

// only lower 32 bits of state are used, but on 64-bit platforms we want uintptr_t to keep
// structures aligned
#define LCG_MAX (0x7ffffffe)
static unsigned int lcgstate;
static bool lcgstate_init;
int lcg_random(void)
{
    if (!lcgstate_init) {
        lcgstate      = rand();
        lcgstate_init = true;
    }
    return ((lcgstate = lcgstate * 1103515245 + 12345) % ((unsigned int)LCG_MAX + 1));
}