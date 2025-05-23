#include "features.h"
#include <cx/string.h>

static void _registerFeature(FeatureRegistry* reg, SubspaceFeature* feat)
{
    // load enabled state from saved settings
    string epath = 0;
    strNConcat(&epath, _S"feature/", feat->name, _S"/enabled");
    ssdCopyOut(reg->ss->settings, epath, bool, &feat->enabled);
    strNConcat(&epath, _S"feature/", feat->name, _S"/available");
    ssdCopyOut(reg->ss->settings, epath, bool, &feat->available);
    strDestroy(&epath);

    featureApplyDefaultSettings(feat);

    htInsertC(&reg->features, string, feat->name, object, &feat);
}
#define registerFeature(reg, feat) _registerFeature(reg, SubspaceFeature(feat))

void registerAllFeatures(FeatureRegistry *reg)
{
    Subspace* ss = reg->ss;

    registerFeature(reg, frameadvCreate(ss));
    registerFeature(reg, infoblockCreate(ss));
    registerFeature(reg, numerichullCreate(ss));
    registerFeature(reg, timewarpCreate(ss));
    registerFeature(reg, tweaksCreate(ss));
}