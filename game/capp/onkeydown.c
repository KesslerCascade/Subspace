#include "capp.h"

int subspace_CApp_OnKeyDown_pre(CApp* self, int key)
{
    return 1;   // we do want to execute the original CApp::OnKeyDown
}