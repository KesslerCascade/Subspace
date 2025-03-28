// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "logfile.h"
#include <cx/fs/path.h>
#include <cx/container.h>
#include <cx/time.h>
#include <cx/format.h>
#include <cx/string.h>
#include <cx/utils.h>

typedef struct LogFileData {
    LogFileConfig config;
    VFS *vfs;
    string fname;
    string pathname;
    string basename;
    string ext;
    VFSFile *curfile;
    int numseen;
    int64 lastrotate;
    int64 cursize;
    uint32 lastbatch;
} LogFileData;

#ifdef _PLATFORM_WIN
static strref loglineend = (strref)"\xE1\xC1\x02""\r\n";
#else
static strref loglineend = (strref)"\xE1\xC1\x01""\n";
#endif

static void deleteOldFiles(_Inout_ LogFileData *lfd);

static void logfileDestroy(_Pre_valid_ _Post_invalid_ LogFileData *data)
{
    vfsClose(data->curfile);
    objRelease(&data->vfs);
    strDestroy(&data->fname);
    strDestroy(&data->pathname);
    strDestroy(&data->basename);
    strDestroy(&data->ext);
    xaFree(data);
}

static bool logfileOpen(_Inout_ LogFileData *data)
{
    devAssert(!data->curfile);
    data->curfile = vfsOpen(data->vfs, data->fname, FS_Create | FS_Write);
    if (!data->curfile)
        return false;
    vfsSeek(data->curfile, 0, FS_End);
    data->cursize = vfsTell(data->curfile);

    return true;
}

static bool logfileClose(_Inout_ LogFileData *data)
{
    devAssert(data->curfile);
    vfsClose(data->curfile);
    data->curfile = NULL;

    return true;
}

_Use_decl_annotations_
LogFileData *logfileCreate(VFS *vfs, strref filename, LogFileConfig *config)
{
    LogFileData *ret = xaAlloc(sizeof(LogFileData), XA_Zero);
    string realfile = 0;
    if (!ret)
        return NULL;

    vfsAbsolutePath(vfs, &realfile, filename);

    ret->config = *config;
    ret->vfs = objAcquire(vfs);
    strDup(&ret->fname, realfile);

    // save path breakdown to make rotation easier
    pathGetExt(&ret->ext, realfile);
    pathParent(&ret->pathname, realfile);
    pathFilename(&ret->basename, realfile);
    pathRemoveExt(&ret->basename, ret->basename);
    strDestroy(&realfile);

    if (strEmpty(ret->ext)) {
        pathAddExt(&ret->fname, ret->fname, _S"log");
        strDup(&ret->ext, _S"log");
    }

    // for time-based rotation need to figure out the last rotate date based on
    // the modify timestamp to make sure it works correctly across sessions
    FSStat stat;
    if (config->rotateMode == LOG_RotateTime && vfsStat(vfs, ret->fname, &stat) == FS_File) {
        if (config->flags & LOG_LocalTime)
            stat.modified = timeLocal(stat.modified, NULL);

        // calculate the last rotation time before the file's modify timestamp
        TimeParts tp;
        timeDecompose(&tp, stat.modified);
        tp.hour = config->rotateHour;
        tp.minute = config->rotateMinute;
        tp.second = config->rotateSecond;
        ret->lastrotate = timeCompose(&tp);

        // if the file was modified before the rotation time, assume it must have been rotated the previous day
        if (stat.modified < ret->lastrotate)
            ret->lastrotate -= timeS(86400);
    }

    if (!logfileOpen(ret)) {
        logfileDestroy(ret);
        return NULL;
    }

    // clear out any old rotated files
    deleteOldFiles(ret);

    return ret;
}

_Use_decl_annotations_
static void deleteOldFiles(LogFileData *lfd)
{
    FSSearchIter fsi;
    string pattern = 0, temp = 0;
    sa_string todelete, splits;

    // now must be in UTC because it's being compared to the file timestamps
    int64 now = clockWall();

    strNConcat(&pattern, lfd->basename, _S".*.", lfd->ext);
    saInit(&todelete, string, 4);
    saInit(&splits, string, 4);

    // count the number of rotated (NON-DATE) files so that unlimited retention can work...
    // unlimited up to 10k files anyway
    lfd->numseen = 0;

    vfsSearchInit(&fsi, lfd->vfs, lfd->pathname, pattern, FS_File, true);
    while (vfsSearchValid(&fsi)) {
        if (lfd->config.rotateKeepTime > 0 &&
            (now - fsi.stat.modified) > lfd->config.rotateKeepTime) {
            saPush(&todelete, string, fsi.name);
        } else if (strSplit(&splits, fsi.name, _S".", false) == 3) {
            int num;
            if (strToInt32(&num, splits.a[1], 10, true) && num < 10000) {
                // this is a numbered file, not one with a date
                // deletion is usually handled at rotation time, but check anyway in case
                // the config changed
                if (lfd->config.rotateKeepFiles > 0 && num > lfd->config.rotateKeepFiles) {
                    saPush(&todelete, string, fsi.name);
                } else {
                    lfd->numseen = max(lfd->numseen, num);
                }
            }
        }
        vfsSearchNext(&fsi);
    }
    vfsSearchFinish(&fsi);

    foreach(sarray, idx, string, fn, todelete) {
        pathJoin(&temp, lfd->pathname, fn);
        vfsDelete(lfd->vfs, temp);
    }

    saDestroy(&todelete);
    saDestroy(&splits);
    strDestroy(&pattern);
    strDestroy(&temp);
}

static void doSizeRotation(_Inout_ LogFileData *lfd)
{
    if (lfd->config.rotateSize == 0 || lfd->cursize < lfd->config.rotateSize)
        return;         // file isn't big enough yet

    logfileClose(lfd);
    deleteOldFiles(lfd);

    int nfiles = lfd->config.rotateKeepFiles > 0 ? lfd->config.rotateKeepFiles : lfd->numseen + 1;

    string namei = 0, nameimo = 0;
    for (int i = nfiles; i >= 1; --i) {
        strFormat(&namei, _S"${string}/${string}.${int}.${string}",
                  stvar(string, lfd->pathname),
                  stvar(string, lfd->basename),
                  stvar(int32, i),
                  stvar(string, lfd->ext));
        if (i > 1) {
            strFormat(&nameimo, _S"${string}/${string}.${int}.${string}",
                      stvar(string, lfd->pathname),
                      stvar(string, lfd->basename),
                      stvar(int32, i - 1),
                      stvar(string, lfd->ext));
        } else {
            strDup(&nameimo, lfd->fname);
        }

        if (vfsExist(lfd->vfs, namei))
            vfsDelete(lfd->vfs, namei);
        if (vfsExist(lfd->vfs, nameimo))
            vfsRename(lfd->vfs, nameimo, namei);
    }
    logfileOpen(lfd);

    strDestroy(&namei);
    strDestroy(&nameimo);
}

static void doTimeRotation(_Inout_ LogFileData *lfd)
{
    int64 lastrotate = lfd->lastrotate;

    // now must be in local time because that's how rotateTime is specified when LOG_LocalTime is set
    int64 now = (lfd->config.flags & LOG_LocalTime) ? clockWallLocal() : clockWall();

    // figure out next rotation time, which will time from the config
    // on the day after the last rotation
    TimeParts tp;
    timeDecompose(&tp, lastrotate + timeS(86400));

    tp.hour = lfd->config.rotateHour;
    tp.minute = lfd->config.rotateMinute;
    tp.second = lfd->config.rotateSecond;
    int64 nextrotate = timeCompose(&tp);

    if (now < nextrotate)
        return;         // not time to rotate yet

    logfileClose(lfd);

    // use last rotation time for date in filename
    timeDecompose(&tp, lastrotate);
    deleteOldFiles(lfd);
    lfd->lastrotate = now;

    string rfname = 0;
    strFormat(&rfname, _S"${string}/${string}.${int}.${string}",
              stvar(string, lfd->pathname),
              stvar(string, lfd->basename),
              stvar(int32, tp.year * 10000 + tp.month * 100 + tp.day),
              stvar(string, lfd->ext));
    vfsRename(lfd->vfs, lfd->fname, rfname);
    strDestroy(&rfname);

    logfileOpen(lfd);
}

static void checkRotate(_Inout_ LogFileData *lfd)
{
    if (lfd->config.rotateMode == LOG_RotateSize)
        doSizeRotation(lfd);
    else if (lfd->config.rotateMode == LOG_RotateTime)
        doTimeRotation(lfd);
}

static void formatDate(_In_ LogFileData *lfd, _Inout_ string *out, int64 timestamp)
{
    int64 toffsetraw = 0;
    TimeParts tp = { 0 };
    if (lfd->config.flags & LOG_LocalTime) {
        timestamp = timeLocal(timestamp, &toffsetraw);
    }

    int toffset = (int32)timeToSeconds(toffsetraw) / 60;        // need offset in minutes for formatting
    timeDecompose(&tp, timestamp);

    switch (lfd->config.dateFormat) {
    case LOG_DateISO:
        if (toffset != 0) {
            // ISO8601 with time zone
            strFormat(out, _S"${0int(4)}-${0uint(2)}-${0uint(2)}T${0uint(2)}:${0uint(2)}:${0uint(2)}${+int(min:2)}:${0int(2)}",
                      stvar(int32, tp.year),
                      stvar(uint8, tp.month),
                      stvar(uint8, tp.day),
                      stvar(uint8, tp.hour),
                      stvar(uint8, tp.minute),
                      stvar(uint8, tp.second),
                      stvar(int32, toffset / 60),
                      stvar(int32, (toffset >= 0 ? toffset : -toffset) % 60));
        } else {
            // ISO8601 with zulu time
            strFormat(out, _S"${0int(4)}-${0uint(2)}-${0uint(2)}T${0uint(2)}:${0uint(2)}:${0uint(2)}Z",
                      stvar(int32, tp.year),
                      stvar(uint8, tp.month),
                      stvar(uint8, tp.day),
                      stvar(uint8, tp.hour),
                      stvar(uint8, tp.minute),
                      stvar(uint8, tp.second));
        }
        break;
    case LOG_DateISOCompact:
        // simplifed ISO-like format with no time zone
        strFormat(out, _S"${0int(4)}-${0uint(2)}-${0uint(2)} ${0uint(2)}:${0uint(2)}:${0uint(2)}",
                  stvar(int32, tp.year),
                  stvar(uint8, tp.month),
                  stvar(uint8, tp.day),
                  stvar(uint8, tp.hour),
                  stvar(uint8, tp.minute),
                  stvar(uint8, tp.second));
        break;
    case LOG_DateNCSA:
        // NCSA common log date format
        strFormat(out, _S"${0uint(2)}/${string(3)}/${0int(4)}:${0uint(2)}:${0uint(2)}:${0uint(2)} ${+int(min:2)}${0int(2)}",
                  stvar(uint8, tp.day),
                  stvar(strref, timeMonthAbbrev[tp.month]),
                  stvar(int32, tp.year),
                  stvar(uint8, tp.hour),
                  stvar(uint8, tp.minute),
                  stvar(uint8, tp.second),
                  stvar(int32, toffset / 60),
                  stvar(int32, (toffset >= 0 ? toffset : -toffset) % 60));
        break;
    case LOG_DateSyslog:
        // BSD-style syslog format (without year)
        strFormat(out, _S"${string(3)} ${uint(2)} ${0uint(2)}:${0uint(2)}:${0uint(2)}",
                  stvar(strref, timeMonthAbbrev[tp.month]),
                  stvar(uint8, tp.day),
                  stvar(uint8, tp.hour),
                  stvar(uint8, tp.minute),
                  stvar(uint8, tp.second));
        break;
    case LOG_DateISOCompactMsec:
        // simplifed ISO-like format with no time zone and milliseconds
        strFormat(out, _S"${0int(4)}-${0uint(2)}-${0uint(2)} ${0uint(2)}:${0uint(2)}:${0uint(2)}.${0uint(3)}",
                  stvar(int32, tp.year),
                  stvar(uint8, tp.month),
                  stvar(uint8, tp.day),
                  stvar(uint8, tp.hour),
                  stvar(uint8, tp.minute),
                  stvar(uint8, tp.second),
                  stvar(uint32, tp.usec / 1000));
        break;
    }

}

// this function is always called from the log thread and does not need to worry about concurrency
_Use_decl_annotations_
void logfileDest(int level, LogCategory *cat, int64 timestamp, strref msg, uint32 batchid, void *userdata)
{
    LogFileData *lfd = (LogFileData*)userdata;
    if (!lfd)
        return;

    if (level == -1) {
        // we're being asked to close the log and quit
        logfileDestroy(lfd);
        return;
    }

    // don't rotate log files in the middle of a batch
    if (batchid != lfd->lastbatch)
        checkRotate(lfd);
    lfd->lastbatch = batchid;

    string logline = 0;
    string logdate = 0, loglevel = 0, logcat = 0, logspaces = 0;

    int nspaces = lfd->config.spacing ? lfd->config.spacing : 2;
    uint8 *sbuf = strBuffer(&logspaces, nspaces + (lfd->config.flags & LOG_AddColon ? 1 : 0));
    memset(sbuf, ' ', nspaces);
    if (lfd->config.flags & LOG_AddColon)
        sbuf[0] = ':';

    formatDate(lfd, &logdate, timestamp);

    // add level prefix
    if (!(lfd->config.flags & LOG_OmitLevel)) {
        strref *lvarr = (lfd->config.flags & LOG_ShortLevel) ? LogLevelAbbrev : LogLevelNames;
        int lvmaxlen = (lfd->config.flags & LOG_ShortLevel) ? 1 : 7;
        if (lfd->config.flags & LOG_BracketLevel) {
            if (lfd->config.flags & LOG_JustifyLevel) {
                // justified with brackets... yuck
                int llen = strLen(lvarr[level]);
                uint8 *temp = strBuffer(&loglevel, lvmaxlen + 3);
                memset(temp, ' ', (size_t)lvmaxlen + 3);
                temp[1] = '[';
                temp[llen + 2] = ']';
                memcpy(temp + 2, strC(lvarr[level]), llen);
            } else {
                strFormat(&loglevel, _S" [${string}]", stvar(strref, lvarr[level]));
            }
        } else if (lfd->config.flags & LOG_JustifyLevel) {
            if (lfd->config.flags & LOG_ShortLevel) {
                strConcat(&loglevel, _S" ", lvarr[level]);
            } else {
                strFormat(&loglevel, _S" ${string(7)}", stvar(strref, lvarr[level]));
            }
        } else {
            strConcat(&loglevel, _S" ", lvarr[level]);
        }
    }

    if (lfd->config.flags & LOG_IncludeCategory && cat && !strEmpty(cat->name)) {
        if (lfd->config.flags & LOG_BracketCategory) {
            strFormat(&logcat, _S" [${string}]", stvar(strref, cat->name));
        } else {
            strConcat(&logcat, _S" ", cat->name);
        }
    }

    if (lfd->config.flags & LOG_CategoryFirst)
        strNConcat(&logline, logdate, logcat, loglevel, logspaces, msg, loglineend);
    else
        strNConcat(&logline, logdate, loglevel, logcat, logspaces, msg, loglineend);
    strDestroy(&logdate);
    strDestroy(&loglevel);
    strDestroy(&logcat);
    strDestroy(&logspaces);

    vfsWrite(lfd->curfile, (void*)strC(logline), strLen(logline), NULL);
    lfd->cursize += strLen(logline);
    strDestroy(&logline);
}
