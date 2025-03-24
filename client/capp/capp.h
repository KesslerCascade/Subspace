#pragma once

#include "ftl/ftl.h"

typedef struct CApp CApp;
extern CApp* theApp;

int subspace_CApp_OnExecute_pre(CApp* self);
