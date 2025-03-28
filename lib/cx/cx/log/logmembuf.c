// absolutely NEVER debug locks in this file because lock debugging calls log*
#ifdef CX_LOCK_DEBUG
#undef CX_LOCK_DEBUG
#endif

#include "logmembuf.h"
#include <cx/time.h>
#include <cx/format.h>

_Use_decl_annotations_
LogMembufData *logmembufCreate(uint32 size)
{
    LogMembufData *ret = xaAlloc(sizeof(LogMembufData), XA_Zero);
    ret->size = size;
    ret->buf = xaAlloc(size, XA_Zero);
    return ret;
}

// for use with logRegisterDest along with the userdata returned from logmembufCreate
_Use_decl_annotations_
void logmembufDest(int level, LogCategory *cat, int64 timestamp, strref msg, uint32 batchid, void *userdata)
{
    LogMembufData *lmd = (LogMembufData*)userdata;
    if (!lmd)
        return;

    if (level == -1) {
        // closing log
        xaFree(lmd->buf);
        xaFree(lmd);
        return;
    }

    TimeParts tp = { 0 };
    timeDecompose(&tp, timestamp);

    string logline = 0, logcat = 0;
    if (cat && !strEmpty(cat->name)) {
        strFormat(&logcat, _S" [${string}]", stvar(strref, cat->name));
    }

    strFormat(&logline, _S"${0int(4)}${0uint(2)}${0uint(2)} ${0uint(2)}${0uint(2)}${0uint(2)} ${string}${string}: ${string}\n",
              stvar(int32, tp.year),
              stvar(uint8, tp.month),
              stvar(uint8, tp.day),
              stvar(uint8, tp.hour),
              stvar(uint8, tp.minute),
              stvar(uint8, tp.second),
              stvar(strref, LogLevelAbbrev[level]),
              stvar(string, logcat),
              stvar(strref, msg));

    uint32 len = strLen(logline);
    if (len < lmd->size) {
        if (len + 1 < lmd->size - lmd->cur) {
            strCopyOut(logline, 0, (uint8*)lmd->buf + lmd->cur, lmd->size - lmd->cur);
            lmd->cur += len;
        } else {
            // overflow, go back to the beginning
            strCopyOut(logline, 0, (uint8*)lmd->buf, lmd->size);
            lmd->cur = len;
        }
    }

    strDestroy(&logcat);
    strDestroy(&logline);
}
