#include "hook/symbol.h"
#include "patch/patch.h"
#include "subspacegame.h"

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
    if (feat->patch) {
        if (!feat->patch(feat, feat->settings, ps))
            return false;
    }

    feat->available = true;
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
