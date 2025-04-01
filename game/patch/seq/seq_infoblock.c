#include "seq_infoblock.h"
#include "patch/commandgui/commandgui.h"
#include "patch/textlibrary/textlibrary.h"

PatchSequence InfoBlockPatches = {
    &patch_CommandGui_RenderStatic,
    &patch_TextLibrary_GetText,
    0
};
