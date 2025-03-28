#pragma once

#include "cx/debug/crash.h"
#include "cx/thread/atomic.h"
#include "cx/thread/mutex.h"
#include "cx/container.h"
#include "cx/utils.h"

typedef struct CrashExtraMeta {
    const char *name;
    char *str;
    int val;
    bool version;
} CrashExtraMeta;
saDeclare(CrashExtraMeta);

typedef struct CrashMemRange {
    uintptr start;
    uintptr end;
} CrashMemRange;
saDeclare(CrashMemRange);

extern LazyInitState _dbgCrashInitState;
extern atomic(uint32) _dbgCrashMode;
extern sa_CrashExtraMeta _dbgCrashExtraMeta;
extern sa_CrashMemRange _dbgCrashDumpMem;
extern Mutex _dbgCrashMutex;

void _dbgCrashInit(void *data);
bool _dbgCrashPlatformInit();
bool _dbgCrashTriggerCallbacks(bool after);
