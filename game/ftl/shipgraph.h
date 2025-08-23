#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

// FTL functions & wrappers below

typedef int (*FUNCTYPE(ShipGraph_Restart))();
DECLFUNC(ShipGraph_Restart);
#define ShipGraph_Restart() FCALL(ftlbase, ShipGraph_Restart)
