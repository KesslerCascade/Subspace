#include "feature/feature.h"
#include "feature/runtracker.h"
#include "ftl/storebox.h"
#include "hook/hook.h"
#include "inventory/resources.h"
#include "patch/patchlist.h"
#include "subspacegame.h"

static EventSource storesrc_scrap;
static EventSource storesrc_res;

// ---- Hooks ----------------

int subspace_ItemStoreBox_Purchase_pre(ItemStoreBox* self)
{
    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceSet(Resource, &storesrc_res, _S"Store");
    eventSourceSet(Scrap, &storesrc_scrap, _S"Store");

    return 1;
}

void subspace_ItemStoreBox_Purchase_post(ItemStoreBox* self)
{
    if (RunTracker_feature.enabled) {
        resourceScan();
    }

    eventSourceFinish(Resource, &storesrc_res);
    eventSourceFinish(Resource, &storesrc_scrap);
}

// ---- Patch ----------------

static bool apply(addr_t base, Patch* p, PatchState* ps)
{
    return hookFunction(base,
                        ItemStoreBox_Purchase,
                        subspace_ItemStoreBox_Purchase_pre,
                        subspace_ItemStoreBox_Purchase_post);
}

Patch patch_ItemStoreBox_Purchase = {
    .relevant        = AlwaysRequired,
    .apply           = apply,
    .requiredSymbols = { &SYM(ItemStoreBox_Purchase), 0 }
};
