#pragma once

#include <cx/log/log.h>
#include <cx/fs/vfs.h>

enum LOG_DATE_FORMATS {
    LOG_DateISO,
    LOG_DateISOCompact,
    LOG_DateNCSA,
    LOG_DateSyslog,
    LOG_DateISOCompactMsec
};

enum LOG_FLAGS {
    LOG_LocalTime           =  0x0001,          // timestamps in local time rather than UTC
    LOG_OmitLevel           =  0x0002,          // do not include severity level
    LOG_ShortLevel          =  0x0004,          // Use log level abbreviations
    LOG_BracketLevel        =  0x0008,          // Enclose log level in brackets
    LOG_JustifyLevel        =  0x0010,          // Make the level a column of consistent length
    LOG_IncludeCategory     =  0x0020,          // Include category name
    LOG_BracketCategory     =  0x0040,          // Put category name in brackets
    LOG_AddColon            =  0x0080,          // Add a colon after the prefix
    LOG_CategoryFirst       =  0x0100,          // category comes between date and level instead of at the end
};

enum LOG_ROTATE_MODE {
    LOG_RotateSize = 1,     // Rotate when file size exceeds rotateSize
    LOG_RotateTime,         // Rotate at a specific time of day
};

typedef struct LogFileConfig {
    int dateFormat;
    int rotateMode;
    int spacing;            // spaces between message prefix and message (default: 2)

    uint32 flags;

    int64 rotateSize;
    uint8 rotateHour;
    uint8 rotateMinute;
    uint8 rotateSecond;

    int rotateKeepFiles;    // minimum number of files to keep after rotation
    int64 rotateKeepTime;   // minimum length of time to keep rotated log files
} LogFileConfig;

typedef struct LogFileData LogFileData;

LogFileData *logfileCreate(_Inout_ VFS *vfs, _In_ strref filename, _In_ LogFileConfig *config);

// for use with logRegisterDest along with the userdata returned from logfileCreate
void logfileDest(int level, _In_opt_ LogCategory *cat, int64 timestamp, _In_opt_ strref msg, uint32 batchid, _In_opt_ void *userdata);
