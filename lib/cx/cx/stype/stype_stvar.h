#include "cx/cx.h"

void stDtor_stvar(stype st, stgeneric *stgen, uint32 flags);
intptr stCmp_stvar(stype st, stgeneric stgen1, stgeneric stgen2, uint32 flags);
void stCopy_stvar(stype st, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags);
uint32 stHash_stvar(stype st, stgeneric stgen, uint32 flags);
_Success_(return) _Check_return_
bool stConvert_stvar(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags);
