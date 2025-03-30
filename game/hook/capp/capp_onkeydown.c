#include "ftl/capp.h"
#include "input/keyboard.h"

int subspace_CApp_OnKeyDown_pre(CApp* self, int key)
{
    if (keyDownGlobal(self, key))
        return 0;
    return 1;   // we do want to execute the original CApp::OnKeyDown
}