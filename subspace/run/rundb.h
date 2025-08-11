#pragma once

#include "subspace.h"
#include "run/runinfo.h"

bool rundbSetAbandoned(Subspace* ss, int64 runid);
bool rundbDelete(Subspace* ss, int64 runid);
