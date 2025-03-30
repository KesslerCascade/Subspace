#include "ftl/commandgui.h"
#include "ftl/shipmanager.h"

extern bool test_frameadv;
int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld)
{
    ShipManager* mgr = CommandGui_ship(self);

    if (key == '[') {
        if (mgr && !ShipManager_GetIsJumping(mgr))
            CommandGui_SetPaused(self, true, false);
        return 0;
    } else if (key == ']') {
        if (mgr && !ShipManager_GetIsJumping(mgr))
            CommandGui_SetPaused(self, false, false);
        return 0;
    } else if (key == '`') {
        if (mgr && !ShipManager_GetIsJumping(mgr)) {
            CommandGui_SetPaused(self, false, false);
            test_frameadv = true;
        }
        return 0;
    }
    return 1;
}