#include "feature.h"
#include "control/controlclient.h"
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

bool validateFeature(SubspaceFeature* feat, PatchState* ps)
{
    if (feat->valid)
        return true;

    // validate extra needed symbols can be resolved
    if (feat->requiredSymbols) {
        for (Symbol** s = feat->requiredSymbols; *s; s++) {
            if (!_symResolve(ps->base, *s))
                goto out;
        }
    }

    // try to validate
    if (feat->requiredPatches) {
        if (!patchValidateSeq(ps, feat->requiredPatches))
            goto out;
    }
    if (feat->validate) {
        if (!feat->validate(feat, ps))
            goto out;
    }

    // everything checked out
    feat->valid = true;

out:
    if (feat->valid) {
        log_fmt(LOG_Verbose, "Feature succesfully validated: %s", feat->name);
    } else {
        log_fmt(LOG_Warn, "Feature failed to validate: %s", feat->name);
    }
    return feat->valid;
}

bool patchFeature(SubspaceFeature* feat, PatchState* ps)
{
    if (feat->available)
        return true;

    if (!feat->valid)
        return false;

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
        log_fmt(LOG_Verbose, "Feature succesfully patched: %s", feat->name);
    } else {
        log_fmt(LOG_Warn, "Feature failed to patch: %s", feat->name);
    }
    return feat->available;
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

    // update controlserver
    sendFeatureState(feat, 0);

    return feat->enabled;
}

void validateAllFeatures(PatchState* ps)
{
    bool ret = true;
    for (uint32_t i = 0; i < feathash.nslots; i++) {
        SubspaceFeature* feat = hashtbl_get_slot(&feathash, i);
        if (feat)
            validateFeature(feat, ps);
    }
}

void patchAllFeatures(PatchState* ps)
{
    bool ret = true;
    for (uint32_t i = 0; i < feathash.nslots; i++) {
        SubspaceFeature* feat = hashtbl_get_slot(&feathash, i);
        if (feat)
            patchFeature(feat, ps);
    }
}

void sendFeatureState(SubspaceFeature* feat, int replyto)
{
    ControlMsg* msg  = controlNewMsg("FeatureState", 3);
    msg->hdr.replyid = replyto;
    controlMsgStr(msg, 0, "feature", feat->name);
    controlMsgBool(msg, 1, "available", feat->available);
    controlMsgBool(msg, 2, "enabled", feat->enabled);
    controlClientQueue(msg);
}

void sendAllFeatureState()
{
    for (uint32_t i = 0; i < feathash.nslots; i++) {
        SubspaceFeature* feat = hashtbl_get_slot(&feathash, i);
        if (feat) {
            sendFeatureState(feat, 0);
        }
    }
}

void fillValidateFeatures(ControlField* featf)
{
    for (uint32_t i = 0; i < feathash.nslots; i++) {
        SubspaceFeature* feat = hashtbl_get_slot(&feathash, i);
        if (feat && feat->available) {
            featf->d.cfd_str_arr               = srealloc(featf->d.cfd_str_arr,
                                            sizeof(char*) * (featf->count + 1));
            featf->d.cfd_str_arr[featf->count] = sstrdup(feat->name);
            featf->count++;
        }
    }
}
