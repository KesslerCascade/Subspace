#include "ftl/commandgui.h"
#include "ftl/shipmanager.h"
#include "timewarp/timewarp.h"

extern bool test_frameadv;
int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld)
{
    ShipManager* mgr = CommandGui_ship(self);

    if (key == '[') {
        if (mgr && !ShipManager_GetIsJumping(mgr))
            timeWarpDecrease();
        return 0;
    } else if (key == ']') {
        if (mgr && !ShipManager_GetIsJumping(mgr))
            timeWarpIncrease();
        return 0;
    } else if (key == '\\') {
        if (mgr && !ShipManager_GetIsJumping(mgr))
            timeWarpEnd();
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