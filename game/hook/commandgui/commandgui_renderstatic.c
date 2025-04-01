#include "ftl/commandgui.h"
#include "feature/infoblock.h"

void subspace_CommandGui_RenderStatic_post(CommandGui* self)
{
    if (InfoBlock_feature.enabled)
        infoBlockRender();
}