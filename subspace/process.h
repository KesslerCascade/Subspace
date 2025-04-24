#pragma once
#include <cx/container.h>
#include <cx/cx.h>
#include <cx/platform.h>

#ifdef _PLATFORM_WIN
typedef void* ProcessHandle;
#endif
saDeclare(ProcessHandle);

bool procCloseHandle(ProcessHandle* h);
bool procOpenByID(ProcessHandle* out, uint32 id);
bool procGetID(ProcessHandle* h, uint32* id);
bool procGetName(ProcessHandle* h, string* out);
bool procRunning(ProcessHandle* h);

ProcessHandle* procRun(string exe, sa_string args);

typedef struct ProcWatchState ProcWatchState;
ProcWatchState* procStartWatchThread();
typedef void (*procExitCb)(ProcessHandle* h, uint32 pid, void* userdata);
bool procWatchAdd(ProcWatchState* pws, ProcessHandle* h, procExitCb closecb, void* userdata);
bool procStopWatchThread(ProcWatchState* pws);
