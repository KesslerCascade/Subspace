#pragma once

#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

// FTL functions & wrappers below

typedef void (*FUNCTYPE(ProjectileFactory_Update))(ProjectileFactory* self);
DECLFUNC(ProjectileFactory_Update);

DECLSYM(ProjectileFactory_blueprint_offset);
#define ProjectileFactory_blueprint(pf) \
    (MEMBER(ftlbase, ProjectileFactory, pf, WeaponBlueprint*, blueprint))