#include <cx/platform/uievent.h>
#include <cx/platform/win.h>
#include <cx/time/time.h>

typedef struct UIEvent {
    HANDLE h;
} UIEvent;

UIEvent *uieventCreate()
{
    UIEvent *ret = xaAlloc(sizeof(UIEvent));
    ret->h = CreateSemaphore(NULL, 0, INT_MAX, NULL);
    return ret;
}

bool uieventSignal(UIEvent *e, int count)
{
    return ReleaseSemaphore(e->h, count, NULL);
}

int uieventWaitTimeout(UIEvent *e, uint64 timeout)
{
    DWORD ret = MsgWaitForMultipleObjects(1, &e->h, FALSE, timeout == timeForever ? INFINITE : (DWORD)timeToMsec(timeout), QS_ALLINPUT);
    if (ret == WAIT_OBJECT_0)
        return UIEVENT_Event;
    if (ret == WAIT_OBJECT_0 + 1)
        return UIEVENT_UI;
    if (ret == WAIT_TIMEOUT)
        return UIEVENT_Timeout;
    return UIEVENT_Error;
}

void uieventDestroy(UIEvent *e)
{
    CloseHandle(e->h);
    xaFree(e);
}
