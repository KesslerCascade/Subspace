#include "ftl/commandgui.h"
#include "ftl/capp.h"

int subspace_CApp_OnLoop_pre(CApp* self)
{
    return 1;   // we do want to execute the original CApp::OnLoop
}

bool test_frameadv = false;
void subspace_CApp_OnLoop_post(CApp* self)
{
    if (test_frameadv) {
        CommandGui_SetPaused(MEMBER(ftlbase, CApp, self, CommandGui*, gui), true, false);
        test_frameadv = false;
    }
}