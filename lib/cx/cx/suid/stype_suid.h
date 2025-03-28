#pragma once

#include "cx/cx.h"

intptr stCmp_suid(stype st, stgeneric stgen1, stgeneric stgen2, uint32 flags);
_Success_(return) _Check_return_
bool stConvert_suid(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags);
