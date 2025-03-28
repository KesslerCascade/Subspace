// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "log_private.h"
#include <cx/format.h>
#include <cx/string.h>
#include <cx/time.h>

int _log_max_level = -1;
static LogCategory _logDefault;
LogCategory* LogDefault = &_logDefault;

atomic(bool) _log_running;
Mutex _log_op_lock;
Mutex _log_run_lock;
hashtable _log_categories;

typedef struct LogBatchTLS {
    LogEntry *head;
    LogEntry *tail;
    int level;
} LogBatchTLS;
static _Thread_local LogBatchTLS _log_batch;

strref LogLevelNames[LOG_Count] = {
    (strref)"\xE1\xC1\x05""Fatal",
    (strref)"\xE1\xC1\x05""Error",
    (strref)"\xE1\xC1\x04""Warn",
    (strref)"\xE1\xC1\x06""Notice",
    (strref)"\xE1\xC1\x04""Info",
    (strref)"\xE1\xC1\x07""Verbose",
    (strref)"\xE1\xC1\x04""Diag",
    (strref)"\xE1\xC1\x05""Debug",
    (strref)"\xE1\xC1\x05""Trace"
};

strref LogLevelAbbrev[LOG_Count] = {
    (strref)"\xE1\xC1\x01""F",
    (strref)"\xE1\xC1\x01""E",
    (strref)"\xE1\xC1\x01""W",
    (strref)"\xE1\xC1\x01""N",
    (strref)"\xE1\xC1\x01""I",
    (strref)"\xE1\xC1\x01""V",
    (strref)"\xE1\xC1\x01""D",
    (strref)"\xE1\xC1\x01""D",
    (strref)"\xE1\xC1\x01""T"
};

LazyInitState _logInitState;
static void logInit(void *dummy)
{
    devAssert(atomicLoad(bool, &_log_running, Acquire) == false);

    saInit(&_log_dests, ptr, 8);
    htInit(&_log_categories, ptr, none, 8);
    mutexInit(&_log_op_lock);
    prqInitDynamic(&_log_queue, LOG_INITIAL_QUEUE_SIZE, LOG_INITIAL_QUEUE_SIZE * 2, LOG_MAX_QUEUE_SIZE, PRQ_Grow_100, PRQ_Grow_100);
    logThreadCreate();

    atomicStore(bool, &_log_running, true, Release);
}

void logCheckInit(void)
{
    lazyInit(&_logInitState, logInit, NULL);
}

_Use_decl_annotations_
void logDestroyEnt(LogEntry *ent)
{
    strDestroy(&ent->msg);
    xaFree(ent);
}

_Use_decl_annotations_
LogCategory *logCreateCat(strref name, bool priv)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return NULL;

    LogCategory *ret = xaAlloc(sizeof(LogCategory), XA_Zero);
    strDup(&ret->name, name);
    ret->priv = priv;

    withMutex (&_log_op_lock) {
        htInsert(&_log_categories, ptr, ret, none, NULL);
    }
    return ret;
}

static void _logStrInternal(int level, _In_ LogCategory *cat, _In_ strref str)
{
    LogEntry *ent = xaAlloc(sizeof(LogEntry), XA_Zero | XA_Optional(High));
    if (!ent)
        return;

    ent->timestamp = clockWall();
    ent->level = level;
    ent->cat = cat;
    strDup(&ent->msg, str);

    if (!_log_batch.level) {
        // to the global log buffer
        logQueueAdd(ent);
    } else {
        // this thread is preparing a batch
        if (_log_batch.tail) {
            _log_batch.tail->_next = ent;
            _log_batch.tail = ent;
        } else {
            _log_batch.head = ent;
            _log_batch.tail = ent;
        }
    }
}

_Use_decl_annotations_
void _logStr(int level, LogCategory *cat, strref str)
{
    lazyInit(&_logInitState, logInit, NULL);

    // early out if no destinations are listening for this log level
    if (level > _log_max_level)
        return;

    if (!atomicLoad(bool, &_log_running, Acquire))
        return;

    _logStrInternal(level, cat, str);
}

_Use_decl_annotations_
void _logFmt(int level, LogCategory *cat, strref fmtstr, int n, stvar *args)
{
    lazyInit(&_logInitState, logInit, NULL);

    // early out if no destinations are listening for this log level
    if (level > _log_max_level)
        return;

    if (!atomicLoad(bool, &_log_running, Acquire))
        return;

    string logmsg = 0;
    _strFormat(&logmsg, fmtstr, n, args);
    _logStrInternal(level, cat, logmsg);
    strDestroy(&logmsg);
}

void logBatchBegin(void)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return;
    
    _log_batch.level++;
}

void logBatchEnd(void)
{
    logCheckInit();
    if (!atomicLoad(bool, &_log_running, Acquire))
        return;

    devAssert(_log_batch.level > 0);
    if (--_log_batch.level == 0) {
        logQueueAdd(_log_batch.head);
        _log_batch.head = NULL;
        _log_batch.tail = NULL;
    }
}

void logShutdown(void)
{
    // Implementation note: Normally the log system is initialized by lazy init. Once shut down,
    // however, the lazy init won't run again and logging will not function. The system can be
    // manually restarted by calling logRestart().

    logCheckInit();

    withMutex (&_log_run_lock) {
        if (!atomicLoad(bool, &_log_running, Acquire))
            break;

        logFlush();

        withMutex (&_log_op_lock) {
            // remove all log destinations
            foreach (sarray, idx, LogDest*, dest, _log_dests) {
                dest->func(-1, NULL, 0, NULL, 0, dest->userdata);
                xaFree(dest);
            }
            saDestroy(&_log_dests);
            _log_max_level = -1;
            
            // remove all saved categories
            foreach(hashtable, hti, _log_categories) {
                xaFree(htiKey(ptr, hti));
            }
            htDestroy(&_log_categories);
        }

        logFlush();

        // shut down log thread
        thrRequestExit(_log_thread);
        thrWait(_log_thread, timeS(10));
        thrRelease(&_log_thread);

        prqDestroy(&_log_queue);

        atomicStore(bool, &_log_running, false, Release);
    }
}

void logRestart(void)
{
     logCheckInit();

    withMutex (&_log_run_lock) {
        if (atomicLoad(bool, &_log_running, Acquire))
            break;

        // Log system was initially started by lazy init, then shut down later.
        // To restart it, we call the init function again but with the run lock held,
        // preventing a race with another shutdown.
        logInit(NULL);
    }
}