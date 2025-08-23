#pragma once

#include "feature/featureregistry.h"

#include "feature/frameadv/frameadv.h"
#include "feature/infoblock/infoblock.h"
#include "feature/numerichull/numerichull.h"
#include "feature/practicemode/practicemode.h"
#include "feature/runtracker/runtracker.h"
#include "feature/savemanager/savemanager.h"
#include "feature/seededruns/seededruns.h"
#include "feature/timewarp/timewarp.h"
#include "feature/tweaks/tweaks.h"

void registerAllFeatures(FeatureRegistry *reg);