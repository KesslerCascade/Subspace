#include "ftl/mousecontrol.h"
#include "subspacegame.h"

// This is just a convenient spot to render random stuff on the screen; since the mouse cursor is
// one of the last things that's drawn
int subspace_MouseControl_OnRender_pre(MouseControl* self)
{
    return 1;
}