#include "subspacegame.h"

extern SubspaceFeature FrameAdv_feature;
extern SubspaceFeature InfoBlock_feature;
extern SubspaceFeature NumericHull_feature;
extern SubspaceFeature TimeWarp_feature;

void registerFeature(SubspaceFeature* feature);
SubspaceFeature* getFeature(const char* name);

bool initFeature(SubspaceFeature* feat, PatchState* ps);
bool enableFeature(SubspaceFeature* feat, bool enabled);
void initAllFeatures(PatchState* ps);
void sendFeatureAvail(void);