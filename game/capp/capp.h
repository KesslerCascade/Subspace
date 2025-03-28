#pragma once

#include "ftl/ftl.h"
#include "ftl/functions_capp.h"
#include "ftl/struct.h"

typedef struct CApp CApp;
extern CApp* theApp;

int subspace_CApp_OnExecute_pre(CApp* self);
int subspace_CApp_OnLoop_pre(CApp* self);
int subspace_CApp_OnLoop_post(int ret, CApp* self);
int subspace_CApp_OnKeyDown_pre(CApp* self, int key);
