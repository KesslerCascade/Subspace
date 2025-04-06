#include <windows.h>
#include "ftl/ftl.h"
#include "ftl/osdep.h"
#include "hook/hook.h"
#include "patch/patch.h"
#include "win32/winmain.h"

static bool validate(addr_t base, Patch* p, PatchState* ps)
{
    return symAddr(base, WinMain) != 0;
}

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, WinMain, subspace_WinMain);
}

Patch patch_OSDepWinMain = { .Relevant = AlwaysRequired, .Validate = validate, .Apply = apply };

PatchSequence OSDepPatches = { &patch_OSDepWinMain, 0 };
