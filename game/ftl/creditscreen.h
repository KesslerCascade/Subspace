#pragma once

#include "ftl/ftl.h"
#include "hook/function.h"

// FTL functions and wrappers below

DECLSYM(CreditScreen_pausing_offset);
#define CreditScreen_pausing(self) MEMBER(ftlbase, CreditScreen, self, float, pausing)
