#pragma once

#include "cx/cx.h"

void stDtor_closure(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *stgen, flags_t flags);
void stCopy_closure(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags);
intptr stCmp_closure(stype st, _In_ stgeneric stgen1, _In_ stgeneric stgen2, uint32 flags);

void stDtor_cchain(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *stgen, flags_t flags);
void stCopy_cchain(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags);
