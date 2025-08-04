#include "feature/feature.h"
#include "feature/tweaks.h"
#include "ftl/soundcontrol.h"
#include "hook/hook.h"
#include "patch/patchlist.h"

// ---- Hooks ----------------

void subspace_SoundControl_StartPlaylist(SoundControl* self, vector* playlist)
{
    if (Tweaks_feature.enabled && tweaksOverrideGameOverMusic(self))
        return;

    SoundControl_StartPlaylist(self, playlist);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return replaceFunction(base, SoundControl_StartPlaylist, subspace_SoundControl_StartPlaylist);
}

Patch patch_SoundControl_StartPlaylist = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(SoundControl_StartPlaylist), 0 }
};
