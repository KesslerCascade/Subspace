#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ResourceControl ResourceControl;

int subspace_ResourceControl_RenderLoadingBar_pre(ResourceControl* self, float initialProgress,
                                                  float finalProgress);

// FTL functions & wrappers below

typedef void (*FUNCTYPE(ResourceControl_RenderLoadingBar))(ResourceControl* self,
                                                           float initialProgress,
                                                           float finalProgress);
DECLFUNC(ResourceControl_RenderLoadingBar);

DECLSYM(ResourceControl_preLoadProgress1_offset);
DECLSYM(ResourceControl_preLoadProgress2_offset);

#define ResourceControl_preLoadProgress1(resource_control) (MEMBER(ftlbase, ResourceControl, resource_control, int, preLoadProgress1))
#define ResourceControl_preLoadProgress2(resource_control) (MEMBER(ftlbase, ResourceControl, resource_control, int, preLoadProgress2))