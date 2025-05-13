#include "keyboard.h"
#include "feature/frameadv.h"
#include "feature/timewarp.h"
#include "keybinds.h"

static bool keyHandleBound(int context, int key, int flags)
{
    bool ret      = false;
    KeyBind* bind = kbGetBinds(key);

    while (bind) {
        if ((!bind->owner || bind->owner->enabled)   // only process binds belonging to an enabled
                                                     // feature (or no feature)
            && bind->context == context              // that are valid in this context
            && ((flags & bind->flags_req) == bind->flags_req)   // have all the required flags
            && ((flags & bind->flags_exc) == 0)                 // and none of the excluded flags
        ) {
            bind->func(key, flags);
            ret = true;
        }

        bind = bind->next;
    }

    return ret;
}

// For both of these, return 'false' to pass the key on to the game itself, and 'true' to intercept
// the key and not pass it on. Note that intercepting in keyDownGlobal will prevent it from being
// passed to CommandGui and bypass keyDownInGame altogether.

bool keyDownGlobal(CApp* app, int key)
{
    return keyHandleBound(KB_CTX_GLOBAL, key, 0);
}

bool keyDownInGame(CommandGui* gui, int key, bool shiftHeld, bool isPaused, bool isJumping)
{
    return keyHandleBound(KB_CTX_GAME,
                          key,
                          (shiftHeld ? KB_SHIFT : 0) | (isPaused ? KB_PAUSED : 0) |
                              (isJumping ? KB_JUMPING : 0));
}
