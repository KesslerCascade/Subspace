#include "ftl/ftlbutton.h"
#include "feature/infoblock.h"

int subspace_FTLButton_OnRender_pre(FTLButton* self)
{
    if (InfoBlock_feature.enabled)
        infoBlockRender();

    return 1;
}