#include "scratch.h"

_Thread_local ScratchPerThreadInfo *_scratch_pti;

_Use_decl_annotations_
extern inline void *scratchGet(size_t sz);
