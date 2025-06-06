#include <windows.h>
#include "ftl/ftl.h"
#include "ftl/osdep.h"
#include "hook/hook.h"
#include "patch/patch.h"
#include "win32/winmain.h"

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, WinMain, subspace_WinMain);
}

Patch patch_OSDepWinMain = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(WinMain), 0 }
};

PatchSequence OSDepPatches = { &patch_OSDepWinMain, 0 };
