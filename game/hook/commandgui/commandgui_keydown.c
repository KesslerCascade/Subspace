#include "ftl/commandgui.h"
#include "ftl/shipmanager.h"

#include "input/keyboard.h"

int subspace_CommandGui_KeyDown_pre(CommandGui* self, int key, bool shiftHeld)
{
    ShipManager* mgr = CommandGui_ship(self);
    bool ispaused    = CommandGui_IsPaused(self);
    bool isjumping   = mgr && ShipManager_GetIsJumping(mgr);

    if (keyDownInGame(self, key, shiftHeld, ispaused, isjumping))
        return 0;

    return 1;
}