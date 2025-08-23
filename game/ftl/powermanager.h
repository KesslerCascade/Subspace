#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

// FTL functions & wrappers below

typedef int (*FUNCTYPE(PowerManager_RestartAll))();
DECLFUNC(PowerManager_RestartAll);
#define PowerManager_RestartAll() FCALL(ftlbase, PowerManager_RestartAll)
