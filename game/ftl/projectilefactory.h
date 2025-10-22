#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

DECLSYM(ProjectileFactory_blueprint_offset);
#define ProjectileFactory_blueprint(pf) \
    (MEMBER(ftlbase, ProjectileFactory, pf, ProjectileBlueprint*, blueprint))