#pragma once

#include <cx/log/log.h>

// memory buffer log target for testing and debugging

typedef struct LogMembufData {
    uint32 size;
    uint32 cur;
    char *buf;
} LogMembufData;

// -------- High level interface

_Ret_valid_
LogMembufData *logmembufCreate(uint32 size);
LogDest* logmembufRegister(int maxlevel, _In_opt_ LogCategory* catfilter,
                           _In_ LogMembufData* membuf);
LogDest* logmembufRegisterWithDefer(int maxlevel, _In_opt_ LogCategory* catfilter,
                                    _In_ LogMembufData* membuf, _In_ LogDest* deferdest);

// -------- Low level interface

// for use with logRegisterDest along with the userdata returned from logmembufCreate
void logmembufMsgFunc(int level, _In_opt_ LogCategory* cat, int64 timestamp, _In_opt_ strref msg,
                      uint32 batchid, _In_opt_ void* userdata);
void logmembufCloseFunc(_In_opt_ void* userdata);