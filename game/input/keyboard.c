#include "keyboard.h"
#include "feature/frameadv.h"
#include "feature/timewarp.h"

// For both of these, return 'false' to pass the key on to the game itself, and 'true' to intercept
// the key and not pass it on. Note that intercepting in keyDownGlobal will prevent it from being
// passed to CommandGui and bypass keyDownInGame altogether.

bool keyDownGlobal(CApp* app, int key)
{
    return false;
}

bool keyDownInGame(CommandGui* gui, int key, bool shiftHeld, bool isPaused, bool isJumping)
{
    if (!isJumping) {
        if (settings.frameAdv->enabled && key == '`') {
            frameAdvStep(gui);
            return true;
        }

        if (settings.timeWarp->enabled) {
            if (key == '[') {
                timeWarpDecrease();
                return true;
            } else if (key == ']') {
                timeWarpIncrease();
                return true;
            } else if (key == '\\') {
                timeWarpEnd();
                return true;
            }
        }
    }

    return false;
}
