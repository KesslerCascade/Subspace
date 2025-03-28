#include "ccallbacks.h"
#include "cx/taskqueue/task/complextask.h"
#include <cx/thread.h>

bool ccbSignalEvent(stvlist* cvars, stvlist* args)
{
    Event* ev = stvlNextPtr(cvars);
    if (!ev)
        return false;

    eventSignal(ev);
    return true;
}

bool ccbSignalSharedEvent(stvlist* cvars, stvlist* args)
{
    SharedEvent* sev = stvlNextPtr(cvars);
    if (!sev)
        return false;

    eventSignal(&sev->ev);
    sheventRelease(&sev);
    return true;
}

bool ccbAdvanceTask(stvlist* cvars, stvlist* args)
{
    Weak(ComplexTask) *ptask;
    if (!stvlNext(cvars, weakref, &ptask))
        return false;
    ComplexTask* ctask = objAcquireFromWeakDyn(ComplexTask, ptask);
    if (!ctask)
        return false;

    ctaskAdvance(ctask);

    objRelease(&ctask);
    return true;
}
 