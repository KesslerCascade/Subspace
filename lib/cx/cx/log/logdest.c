// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "log_private.h"

sa_LogDest _log_dests;

_Use_decl_annotations_
LogDest *logRegisterDest(int maxlevel, LogCategory *catfilter, LogDestFunc dest, void *userdata)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return NULL;

    LogDest *ndest = xaAlloc(sizeof(LogDest), XA_Zero);

    ndest->maxlevel = maxlevel;
    ndest->catfilter = catfilter;
    ndest->func = dest;
    ndest->userdata = userdata;

    mutexAcquire(&_log_op_lock);
    saPush(&_log_dests, ptr, ndest);
    if (maxlevel > _log_max_level)
        _log_max_level = maxlevel;
    mutexRelease(&_log_op_lock);
    return ndest;
}

_Use_decl_annotations_
bool logUnregisterDestLocked(LogDest *dhandle)
{
    bool ret = false;

    // need to recalculate max level from what's left
    _log_max_level = -1;

    for (int i = saSize(_log_dests) - 1; i >= 0; --i) {
        if (_log_dests.a[i] == dhandle) {
            saRemove(&_log_dests, i);
            ret = true;
        } else if (_log_dests.a[i]->maxlevel > _log_max_level) {
            _log_max_level = _log_dests.a[i]->maxlevel;
        }
    }

    return ret;
}

_Use_decl_annotations_
bool logUnregisterDest(LogDest *dhandle)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return false;

    bool ret = false;
    mutexAcquire(&_log_op_lock);
    ret = logUnregisterDestLocked(dhandle);
    mutexRelease(&_log_op_lock);

    // notify dest that it's no longer needed
    dhandle->func(-1, NULL, 0, NULL, 0, dhandle->userdata);
    xaFree(dhandle);
    return ret;
}
