#pragma once
#include "ftl/ftl.h"
#include "ftl/struct.h"
#include "hook/function.h"

typedef struct ShipBlueprint ShipBlueprint;

// FTL functions & wrappers below

DECLSYM(ShipBlueprint_blueprintName_offset);
#define ShipBlueprint_blueprintName(bprint) \
    (&MEMBER(ftlbase, ShipBlueprint, bprint, basic_string, blueprintName))

DECLSYM(ShipBlueprint_name_offset);
#define ShipBlueprint_name(bprint) \
    (&MEMBER(ftlbase, ShipBlueprint, bprint, TextString, blueprintName))
