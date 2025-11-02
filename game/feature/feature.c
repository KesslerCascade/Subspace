#include "feature.h"
#include "control/controlclient.h"
#include "hook/symbol.h"
#include "input/keybinds.h"
#include "log/log.h"
#include "patch/patch.h"
#include "subspacegame.h"

static hashtable feathash;
static bool feathash_init;

void registerFeature(SubspaceFeature* feature)
{
    if (!feathash_init) {
        htInit(&feathash, string, ptr, 8);
        feathash_init = true;
    }

    htInsert(&feathash, strref, (strref)feature->name, ptr, feature);

    // register the feature's keybinds
    KeyBind* bind = feature->keybinds;
    while (bind && bind->name) {
        kbRegisterBind(feature, bind);
        ++bind;
    }
}

void registerAllFeatures()
{
    registerFeature(&Base_feature);
    registerFeature(&InfoBlock_feature);
    registerFeature(&TimeWarp_feature);
    registerFeature(&FrameAdv_feature);
    registerFeature(&NumericHull_feature);
    registerFeature(&PracticeMode_feature);
    registerFeature(&RunTracker_feature);
    registerFeature(&SaveManager_feature);
    registerFeature(&Screenshot_feature);
    registerFeature(&Tweaks_feature);
}

SubspaceFeature* getFeature(const char* name)
{
    void* val = NULL;
    htFind(feathash, strref, (strref)name, ptr, &val);
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
    if (feat->settingsspec && feat->settingsspec->size > 0) {
        feat->settings = xaAlloc(feat->settingsspec->size, XA_Zero);
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
    foreach (hashtable, hti, feathash) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(ptr, hti);
        if (feat)
            validateFeature(feat, ps);
    }
}

void patchAllFeatures(PatchState* ps)
{
    bool ret = true;
    foreach (hashtable, hti, feathash) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(ptr, hti);
        if (feat)
            patchFeature(feat, ps);
    }
}

void sendFeatureState(SubspaceFeature* feat, int replyto)
{
    ControlMsg* msg  = controlNewMsg("FeatureState", 3);
    msg->hdr.replyid = replyto;
    controlMsgStr(msg, 0, "feature", (strref)feat->name);
    controlMsgBool(msg, 1, "available", feat->available);
    controlMsgBool(msg, 2, "enabled", feat->enabled);
    controlClientQueue(msg);
}

void sendAllFeatureState()
{
    foreach (hashtable, hti, feathash) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(ptr, hti);
        if (feat) {
            sendFeatureState(feat, 0);
        }
    }
}

void fillValidateFeatures(ControlField* featf)
{
    foreach (hashtable, hti, feathash) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(ptr, hti);
        if (feat && feat->available) {
            saPush(&featf->d.cfd_str_arr, strref, (strref)feat->name);
            featf->count++;
        }
    }
}
