#include "hook/symbol.h"
#include "log/log.h"
#include "patch/patch.h"
#include "subspacegame.h"

static hashtbl feathash;

void registerFeature(SubspaceFeature* feature)
{
    if (!feathash.ents) {
        hashtbl_init(&feathash, 8, HT_STRING_KEYS);
    }

    hashtbl_add(&feathash, feature->name, feature);
}

SubspaceFeature* getFeature(const char* name)
{
    void* val = hashtbl_get(&feathash, name);
    return (SubspaceFeature*)val;
}

bool initFeature(SubspaceFeature* feat, PatchState* ps)
{
    feat->available = false;

    // validate extra needed symbols can be resolved
    if (feat->requiredSymbols) {
        for (Symbol** s = feat->requiredSymbols; *s; s++) {
            if (!_symResolve(ps->base, *s))
                return false;
        }
    }

    // allocate needed space for settings
    if (feat->settingsSize > 0) {
        feat->settings = smalloc(feat->settingsSize(feat));
        if (!feat->settings)
            return false;
    } else {
        feat->settings = NULL;
    }

    // try to patch
    if (feat->requiredPatches) {
        if (!patchApplySeq(ps, feat->requiredPatches))
            goto out;
    }
    if (feat->patch) {
        if (!feat->patch(feat, feat->settings, ps))
            goto out;
    }

    feat->available = true;
out:
    if (feat->available) {
        log_fmt(LOG_Verbose, "Feature succesfully initialized: %s", feat->name);
    } else {
        log_fmt(LOG_Warn, "Feature failed to patch: %s", feat->name);
    }
    return true;
}

bool enableFeature(SubspaceFeature* feat, bool enabled)
{
    if (!feat->available)
        return false;
    if (enabled == feat->enabled)
        return enabled;   // no change

    if (feat->enable) {
        feat->enabled = feat->enable(feat, feat->settings, enabled);
    } else {
        feat->enabled = enabled;
    }

    return feat->enabled;
}

void initAllFeatures(PatchState* ps)
{
    bool ret = true;
    for (uint32_t i = 0; i < feathash.nslots; i++) {
        SubspaceFeature* feat = hashtbl_get_slot(&feathash, i);
        if (feat) {
            if (!initFeature(feat, ps))
                return;   // memory allocation failure
        }
    }
}
