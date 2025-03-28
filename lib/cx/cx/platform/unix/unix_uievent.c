#include <cx/platform/uievent.h>

UIEvent *uieventCreate()
{
    return NULL;
}

bool uieventSignal(UIEvent *e, int count)
{
    return false;
}

int uieventWaitTimeout(UIEvent *e, uint64 timeout)
{
    return UIEVENT_Error;
}

void uieventDestroy(UIEvent *e)
{
}
