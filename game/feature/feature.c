#include "feature.h"
#include "control/controlclient.h"
#include "hook/symbol.h"
#include "input/keybinds.h"
#include "log/gamelog.h"
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
        logFmt(Verbose,
               _S"Feature succesfully validated: ${string}",
               stvar(strref, (strref)feat->name));
    } else {
        logFmt(Warn, _S"Feature failed to validate: ${string}", stvar(strref, (strref)feat->name));
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
        logFmt(Verbose,
               _S"Feature succesfully patched: ${string}",
               stvar(strref, (strref)feat->name));
    } else {
        logFmt(Warn, _S"Feature failed to patch: ${string}", stvar(strref, (strref)feat->name));
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
    ControlMsg* msg  = controlMsgCreate(_S"FeatureState");
    msg->hdr.replyid = replyto;
    cfieldSet(msg, _S"feature", strref, (strref)feat->name);
    cfieldSet(msg, _S"available", bool, feat->available);
    cfieldSet(msg, _S"enabled", bool, feat->enabled);
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

void fillValidateFeatures(sa_string* featlist)
{
    foreach (hashtable, hti, feathash) {
        SubspaceFeature* feat = (SubspaceFeature*)htiVal(ptr, hti);
        if (feat && feat->available) {
            saPush(featlist, strref, (strref)feat->name);
        }
    }
}
