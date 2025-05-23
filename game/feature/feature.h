#include "subspacegame.h"

extern SubspaceFeature Base_feature;
extern SubspaceFeature FrameAdv_feature;
extern SubspaceFeature InfoBlock_feature;
extern SubspaceFeature NumericHull_feature;
extern SubspaceFeature TimeWarp_feature;
extern SubspaceFeature Tweaks_feature;
typedef struct ControlField ControlField;

void registerFeature(SubspaceFeature* feature);
SubspaceFeature* getFeature(const char* name);

bool validateFeature(SubspaceFeature* feat, PatchState* ps);
bool patchFeature(SubspaceFeature* feat, PatchState* ps);
bool enableFeature(SubspaceFeature* feat, bool enabled);
void registerAllFeatures();
void validateAllFeatures(PatchState* ps);
void patchAllFeatures(PatchState* ps);
void sendFeatureState(SubspaceFeature* feat, int replyto);
void sendAllFeatureState();
void fillValidateFeatures(ControlField* featf);
