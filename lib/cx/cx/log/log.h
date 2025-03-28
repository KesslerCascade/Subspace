#pragma once

#include <cx/cx.h>
#include <cx/string/strbase.h>
#include <cx/stype/stvar.h>

enum LOG_LEVEL_ENUM {
    LOG_Fatal,
    LOG_Error,
    LOG_Warn,
    LOG_Notice,
    LOG_Info,
    LOG_Verbose,
    LOG_Diag,           // Release build diagnostics not normally needed
    LOG_Debug,          // Compiled out of non-development builds
    LOG_Trace,          // Only available in debug builds

    LOG_Count           // Not a real level, for static assertions
};
extern strref LogLevelNames[];
extern strref LogLevelAbbrev[];

typedef struct LogCategory {
    string name;
    bool priv;
} LogCategory;
typedef struct LogDest LogDest;
extern LogCategory* LogDefault;

_Ret_opt_valid_
LogCategory *logCreateCat(_In_ strref name, bool priv);
void _logStr(int level, _In_ LogCategory *cat, _In_ strref str);
void _logFmt(int level, _In_ LogCategory *cat, _In_ strref fmtstr, int n, _In_ stvar *args);

// NOTE: will be called with level == -1 to indicate that the destination has been removed or the
// log system is shutting down. The destination provider should assume it will never be called again
// and close log files, etc.
// A log destination should try to keep messages with the same batchid together, i.e. not split logfiles
// across the batch if possible. The batch ID should be treated as opaque and for comparison purposes only;
// it has no particular ordering.
typedef void(*LogDestFunc)(int level, _In_opt_ LogCategory *cat, int64 timestamp, _In_opt_ strref msg, uint32 batchid, _In_opt_ void *userdata);

_Ret_opt_valid_
LogDest *logRegisterDest(int maxlevel, _In_opt_ LogCategory *catfilter, _In_ LogDestFunc dest, _In_opt_ void *userdata);
bool logUnregisterDest(_Pre_valid_ _Post_invalid_ LogDest *dhandle);
void logFlush(void);

// Flushes all pending logs, unregisters all destinations, invalidates all categories
void logShutdown(void);

// Restarts the logging system after a shutdown
void logRestart(void);

void logBatchBegin(void);
void logBatchEnd(void);

#define logStr(level, str)       _logStr_##level(LOG_##level, LogDefault, str)
#define logStrC(level, cat, str) _logStr_##level(LOG_##level, cat, str)
#define logFmt(level, fmt, ...)       _logFmt_##level(LOG_##level, LogDefault, fmt, count_macro_args(__VA_ARGS__), ((stvar[]){ __VA_ARGS__ }))
#define logFmtC(level, cat, fmt, ...) _logFmt_##level(LOG_##level, cat, fmt, count_macro_args(__VA_ARGS__), ((stvar[]){ __VA_ARGS__ }))

#if DEBUG_LEVEL >= 2
#define _logStr_Trace(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Trace(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#else
#define _logStr_Trace(level, cat, str) ((void)0)
#define _logFmt_Trace(level, cat, fmt, nargs, args) ((void)0)
#endif

#if DEBUG_LEVEL >= 1
#define _logStr_Debug(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Debug(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_DevDiag(level, cat, str) _logStr(LOG_Diag, cat, str)
#define _logFmt_DevDiag(level, cat, fmt, nargs, args) _logFmt(LOG_Diag, cat, fmt, nargs, args)
#define _logStr_DevVerbose(level, cat, str) _logStr(LOG_Verbose, cat, str)
#define _logFmt_DevVerbose(level, cat, fmt, nargs, args) _logFmt(LOG_Verbose, cat, fmt, nargs, args)
#define _logStr_DevInfo(level, cat, str) _logStr(LOG_Info, cat, str)
#define _logFmt_DevInfo(level, cat, fmt, nargs, args) _logFmt(LOG_Info, cat, fmt, nargs, args)
#define _logStr_DevNotice(level, cat, str) _logStr(LOG_Notice, cat, str)
#define _logFmt_DevNotice(level, cat, fmt, nargs, args) _logFmt(LOG_Notice, cat, fmt, nargs, args)
#define _logStr_DevWarn(level, cat, str) _logStr(LOG_Warn, cat, str)
#define _logFmt_DevWarn(level, cat, fmt, nargs, args) _logFmt(LOG_Warn, cat, fmt, nargs, args)
#define _logStr_DevError(level, cat, str) _logStr(LOG_Error, cat, str)
#define _logFmt_DevError(level, cat, fmt, nargs, args) _logFmt(LOG_Error, cat, fmt, nargs, args)
#else
#define _logStr_Debug(level, cat, str) ((void)0)
#define _logFmt_Debug(level, cat, str) ((void)0)
#define _logStr_DevDiag(level, cat, str) ((void)0)
#define _logFmt_DevDiag(level, cat, str) ((void)0)
#define _logStr_DevVerbose(level, cat, str) ((void)0)
#define _logFmt_DevVerbose(level, cat, str) ((void)0)
#define _logStr_DevInfo(level, cat, str) ((void)0)
#define _logFmt_DevInfo(level, cat, str) ((void)0)
#define _logStr_DevNotice(level, cat, str) ((void)0)
#define _logFmt_DevNotice(level, cat, str) ((void)0)
#define _logStr_DevWarn(level, cat, str) ((void)0)
#define _logFmt_DevWarn(level, cat, str) ((void)0)
#define _logStr_DevError(level, cat, str) ((void)0)
#define _logFmt_DevError(level, cat, str) ((void)0)
#endif

#define _logStr_Diag(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Diag(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Verbose(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Verbose(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Info(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Info(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Notice(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Notice(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Warn(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Warn(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Error(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Error(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
#define _logStr_Fatal(level, cat, str) _logStr(level, cat, str)
#define _logFmt_Fatal(level, cat, fmt, nargs, args) _logFmt(level, cat, fmt, nargs, args)
