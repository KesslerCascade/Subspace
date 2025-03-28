#include "capp.h"

int subspace_CApp_OnExecute_pre(CApp* self)
{
    // save the pointer to the CApp instance
    theApp = self;

    return 1;   // we do want to execute the original CApp::OnExecute
}