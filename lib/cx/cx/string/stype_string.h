#pragma once

#include "cx/cx.h"

void stDtor_string(stype st, _Pre_notnull_ _Post_invalid_ stgeneric *stgen, uint32 flags);
intptr stCmp_string(stype st, _In_ stgeneric stgen1, _In_ stgeneric stgen2, uint32 flags);
void stCopy_string(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags);
uint32 stHash_string(stype st, _In_ stgeneric stgen, uint32 flags);

_Success_(return) _Check_return_
bool stConvert_string(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags);
