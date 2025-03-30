#include "feature/frameadv.h"

int subspace_CApp_OnLoop_pre(CApp* self)
{
    return 1;   // we do want to execute the original CApp::OnLoop
}

void subspace_CApp_OnLoop_post(CApp* self)
{
    if (settings.frameAdv->enabled)
        frameAdvEndFrame(self);
}