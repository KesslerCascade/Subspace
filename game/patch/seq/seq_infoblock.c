#include "seq_infoblock.h"
#include "patch/ftlbutton/ftlbutton.h"
#include "patch/textlibrary/textlibrary.h"

PatchSequence InfoBlockPatches = { &patch_FTLButton_OnRender, &patch_TextLibrary_GetText, 0 };
