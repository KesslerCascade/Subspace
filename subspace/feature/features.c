#include "features.h"

void registerAllFeatures(FeatureRegistry *reg)
{
    Subspace* ss = reg->ss;
    SubspaceFeature* f;

    f = SubspaceFeature(frameadvCreate(ss));
    htInsertC(&reg->features, string, f->name, object, &f);
    f = SubspaceFeature(infoblockCreate(ss));
    htInsertC(&reg->features, string, f->name, object, &f);
    f = SubspaceFeature(numerichullCreate(ss));
    htInsertC(&reg->features, string, f->name, object, &f);
    f = SubspaceFeature(timewarpCreate(ss));
    htInsertC(&reg->features, string, f->name, object, &f);
    return;
}