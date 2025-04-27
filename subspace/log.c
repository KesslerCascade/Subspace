#include "subspace.h"
#include <cx/log.h>

#define LOG_SEGMENT_SIZE (1024*1024)
#define LOG_SEGMENTS 5
static LogFileData *logdata;
static LogDest *dest;
LogCategory* gamecat;

bool logOpen(VFS *vfs, string filename, LogDest **defer)
{
    string pdir = 0;
    pathParent(&pdir, filename);
    vfsCreateAll(vfs, pdir);
    strDestroy(&pdir);

    gamecat              = logCreateCat(_S"Game", false);
    LogFileConfig config = {
        .flags           = LOG_LocalTime | LOG_ShortLevel | LOG_BracketLevel,
        .dateFormat      = LOG_DateISOCompact,
        .spacing         = 4,
        .rotateMode      = LOG_RotateSize,
        .rotateSize      = LOG_SEGMENT_SIZE,
        .rotateKeepFiles = LOG_SEGMENTS,
        .rotateKeepTime  = timeS(86400 * 14)
    };
    logdata = logfileCreate(vfs, filename, &config);
    if (!logdata)
        return false;
    dest = logRegisterDestWithDefer(LOG_Verbose, NULL, logfileDest, logdata, *defer);
    *defer = NULL;
    return dest;
}

bool logClose(void)
{
    logFlush();
    logUnregisterDest(dest);
    dest = NULL;
    logdata = NULL;
    return true;
}