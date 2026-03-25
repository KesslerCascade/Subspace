#pragma once
#include "ftl/ftl.h"
#include "hook/function.h"

int subspace_FTLButton_OnRender_pre(FTLButton* self);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(FTLButton_OnRender))(FTLButton* self);
DECLFUNC(FTLButton_OnRender);
