#pragma once

#include <cx/cx.h>

CX_C_BEGIN

enum DEBUG_CRASH_FLAGS_ENUM {
    DBG_CrashExit        = 0x0001,          // exit gracefully
    DBG_CrashDump        = 0x0002,          // generate small memory dump
    DBG_CrashFullDump    = 0x0004,          // generate full memory dump
    DBG_CrashUpload      = 0x0008,          // submit crash report to reporting service
    DBG_CrashBreakpoint  = 0x0010,          // trigger breakpoint
    DBG_CrashDelete      = 0x0020,          // delete dump after successful upload
    DBG_CrashInternal    = 0x0040,          // submit to internal endpoint rather than public
    DBG_CrashProgressUI  = 0x0080,          // show progress UI while uploading
    DBG_CrashDevMode     = 0x0100,          // the process is running in development mode; allow debugging
    DBG_CrashNotify      = 0x0200,          // pop up notification but do not offer options
    DBG_CrashPrompt      = 0x1000,          // prompt user upon crash, will set or clear other flags
    DBG_CrashPromptLocal = 0x3000,          // prompt user upon crash, do not allow upload (implies CrashPrompt)
};

// Some defaults for convenience
#define DBG_CrashInteractive (DBG_CrashExit | DBG_CrashPrompt | DBG_CrashProgressUI)
#define DBG_CrashNonInteractive (DBG_CrashExit | DBG_CrashDump | DBG_CrashUpload | DBG_CrashDelete)

// Set crash mode
void dbgCrashSetMode(uint32 mode);
uint32 dbgCrashGetMode();

// Set crash dump location
bool dbgCrashSetPath(_In_ strref path);

// Callback function when a crash occurs
//     after: false when called the first time (immediately after exception)
//            true when called just before exiting
// WARNING: Do not perform complex actions such as memory allocation in the callback!
// return true to continue processing the crash or false to abort and hand off to OS handler
typedef bool(*dbgCrashCallback)(bool after);

// Install function to call upon crash
void dbgCrashAddCallback(dbgCrashCallback cb);
void dbgCrashRemoveCallback(dbgCrashCallback cb);

// Mark a segment of memory to include in crash dump
void dbgCrashIncludeMemory(void *ptr, size_t sz);
// Remove a segment previously marked. This does not exclude
// regions that are automatically selected such as stacks and
// data segments.
void dbgCrashExcludeMemory(void *ptr, size_t sz);

// Adding custom metadata to crash report
// Should be strictly related to the crash, i.e. just before calling dbgCrashNow.
// For normal runtime metadata the blackbox system should be used instead.
void dbgCrashAddMetaStr(_In_z_ const char *name, _In_z_ const char *val);
void dbgCrashAddMetaInt(_In_z_ const char *name, int val);

// Adding version metadata to crash report
// This goes into the root of the crash report
void dbgCrashAddVersionStr(_In_z_ const char *name, _In_z_ const char *val);
void dbgCrashAddVersionInt(_In_z_ const char *name, int val);

void _no_return dbgCrashNow(int skipframes);

CX_C_END
