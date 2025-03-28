#include "log_private.h"
#include "logdefer.h"

typedef struct LogDeferEntry LogDeferEntry;
typedef struct LogDeferData
{
    LogDeferEntry *head;
    LogDeferEntry *tail;
} LogDeferData;

typedef struct LogDeferEntry
{
    LogDeferEntry *next;
    LogEntry ent;
    uint32 batchid;
} LogDeferEntry;

_Use_decl_annotations_
LogDeferData *logDeferCreate(void)
{
    LogDeferData *ret = xaAllocStruct(LogDeferData, XA_Zero);
    return ret;
}

_Use_decl_annotations_
void logDeferDest(int level, LogCategory *cat, int64 timestamp, strref msg, uint32 batchid, void *userdata)
{
    LogDeferData *dd = (LogDeferData *)userdata;
    if (!dd || level == -1)
        return;

    LogDeferEntry *de = xaAllocStruct(LogDeferEntry, XA_Zero);
    de->ent.level = level;
    de->ent.cat = cat;
    de->ent.timestamp = timestamp;
    de->batchid = batchid;
    strDup(&de->ent.msg, msg);
    if (!dd->head)
        dd->head = de;
    if (dd->tail)
        dd->tail->next = de;
    dd->tail = de;
}

_Use_decl_annotations_
LogDest *logRegisterDestWithDefer(int maxlevel, LogCategory *catfilter, LogDestFunc dest, void *userdata, LogDest *deferdest)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return NULL;

    if (!deferdest) {
        // just a regular registration in this case
        return logRegisterDest(maxlevel, catfilter, dest, userdata);
    }

    LogDest *ndest = xaAlloc(sizeof(LogDest), XA_Zero);

    ndest->maxlevel = maxlevel;
    ndest->catfilter = catfilter;
    ndest->func = dest;
    ndest->userdata = userdata;

    // swap out the new destination for the deferred one
    withMutex(&_log_op_lock) {
        saPush(&_log_dests, ptr, ndest);
        logUnregisterDestLocked(deferdest);     // will recalculate maxlevel cache

        // keep the lock held to ensure that the writer thread doesn't send anything to the new dest before
        // the deferred entries are flushed, and also to preserve the guarantee that LogDestFunc callbacks will
        // never be run from two threads at once.

        // sanity check
        if (deferdest->userdata && deferdest->func == logDeferDest) {
            LogDeferData *dd = (LogDeferData *)deferdest->userdata;
            LogDeferEntry *head = dd->head;
            dd->tail = NULL;
            dd->head = NULL;

            // go through linked list and send them all to the real destination
            LogDeferEntry *next;
            while (head) {
                next = head->next;
                if (head->ent.level <= maxlevel && applyCatFilter(catfilter, head->ent.cat))
                    dest(head->ent.level, head->ent.cat, head->ent.timestamp, head->ent.msg, head->batchid, userdata);
                strDestroy(&head->ent.msg);
                xaFree(head);
                head = next;
            }

            xaFree(dd);
        }
    }

    xaFree(deferdest);

    return ndest;
}
