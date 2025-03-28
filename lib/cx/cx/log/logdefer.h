#pragma once

#include <cx/log/log.h>

// Deferred logging is a special destination that gathers log messages into a temporary batch,
// which is then flushed to a different destination later.
// It's intended for use cases where an application wants to start using logging immediately,
// and still save those logs to a log file after it's opened.

typedef struct LogDeferData LogDeferData;

_Ret_valid_
LogDeferData *logDeferCreate(void);

// for use with logRegisterDest along with the userdata returned from logdeferCreate
void logDeferDest(int level, _In_opt_ LogCategory *cat, int64 timestamp, _In_opt_ strref msg, uint32 batchid, _In_opt_ void *userdata);

// Atomically registers a new destination with the logging system, while also transferring
// the contents of the defer buffer to the new destination. The defer buffer is destroyed
// during the process and may not be re-used.
_Ret_opt_valid_
LogDest *logRegisterDestWithDefer(int maxlevel, _In_opt_ LogCategory *catfilter, _In_ LogDestFunc dest, _In_opt_ void *userdata,
                                  _In_opt_ _Post_invalid_ LogDest *deferdest);
