#include "capp.h"

int subspace_CApp_OnLoop_pre(CApp* self)
{
    return 1;   // we do want to execute the original CApp::OnLoop
}

int subspace_CApp_OnLoop_post(int ret, CApp* self)
{
    return ret;
}