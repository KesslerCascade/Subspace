#pragma once

#include <cx/cx.h>

// Very simple OS-managed event object with wait function that also returns
// early when UI input occurs. Designed to get kernel support for Events
// with EV_UIEvent.

#if defined(_PLATFORM_WIN)
#define _UIEVENT_SUPPORTED
#endif

typedef struct UIEvent UIEvent;

enum UIEVENT_Result {
    UIEVENT_Error   = 0,
    UIEVENT_Event   = 1,        // Event was signaled
    UIEVENT_UI      = 2,        // Other UI input occured
    UIEVENT_Timeout = 3         // timeout reached
};

UIEvent *uieventCreate();
bool uieventSignal(UIEvent *e, int count);
int uieventWaitTimeout(UIEvent *e, uint64 timeout);
void uieventDestroy(UIEvent *e);
