#include "sbfsfile.h"

typedef struct SbufFSFileCtx {
    FSFile *file;
    bool close;
} SbufFSFileCtx;

static void sbufFSFileCleanup(_Pre_valid_ void *ctx)
{
    SbufFSFileCtx *sbc = (SbufFSFileCtx *)ctx;
    if (sbc->close)
        fsClose(sbc->file);
    xaFree(sbc);
}

_Use_decl_annotations_
bool sbufFSFileIn(StreamBuffer *sb, FSFile *file, bool close)
{
    if (!sbufPRegisterPush(sb, NULL, NULL))
        return false;

    uint8 *buf = xaAlloc(sb->targetsz);
    size_t didread = 0;
    for(;;) {
        if (!fsRead(file, buf, sb->targetsz, &didread)) {
            sbufError(sb);
            break;
        }

        if (didread == 0)           // EOF
            break;

        sbufPWrite(sb, buf, didread);
    }
    xaFree(buf);

    if (close)
        fsClose(file);

    bool ret = !sbufIsError(sb);
    sbufPFinish(sb);
    return ret;
}

static size_t sbufFSFilePullCB(_Pre_valid_ StreamBuffer *sb, _Out_writes_bytes_(sz) uint8 *buf, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufFSFileCtx *sbc = (SbufFSFileCtx *)ctx;
    if (!sbc)
        return 0;

    size_t didread = 0;
    if (!fsRead(sbc->file, buf, sz, &didread))
        sbufError(sb);

    if (didread == 0)
        sbufPFinish(sb);

    return didread;
}

_Use_decl_annotations_
bool sbufFSFilePRegisterPull(StreamBuffer *sb, FSFile *file, bool close)
{
    SbufFSFileCtx *sbc = xaAlloc(sizeof(SbufFSFileCtx));
    sbc->file = file;
    sbc->close = close;

    if (!sbufPRegisterPull(sb, sbufFSFilePullCB, sbufFSFileCleanup, sbc)) {
        sbufFSFileCleanup(sbc);
        return false;
    }

    return true;
}

static bool sbufFSFileSendCB(_Pre_valid_ StreamBuffer *sb, _In_reads_bytes_(sz) const uint8 *buf, size_t off, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufFSFileCtx *sbc = (SbufFSFileCtx *)ctx;
    if (!sbc)
        return false;

    size_t didwrite = 0;
    if (!fsWrite(sbc->file, (void*)buf, sz, &didwrite))
        sbufError(sb);

    return true;
}

static void sbufFSFileNotifyCB(_Pre_valid_ StreamBuffer *sb, size_t sz, _Pre_opt_valid_ void *ctx)
{
    if (sz >= (sb->targetsz >> 1) + (sb->targetsz >> 2)) {
        sbufCSend(sb, sbufFSFileSendCB, sz);
    } else if (sz == 0 || sbufIsPFinished(sb)) {
        // flush anything that's left in the streambuf
        sbufCSend(sb, sbufFSFileSendCB, sbufCAvail(sb));
    }
}

_Use_decl_annotations_
bool sbufFSFileOut(StreamBuffer *sb, FSFile *file, bool close)
{
    if (!sbufCRegisterPull(sb, NULL, NULL))
        return false;

    uint8 *buf = xaAlloc(sb->targetsz);
    size_t sz;
    do {
        // grab targetsz at a time from the buffer
        if (sbufCRead(sb, buf, sb->targetsz, &sz)) {
            size_t didwrite;
            if (!fsWrite(file, buf, sz, &didwrite)) {
                sbufError(sb);
                break;
            }
        }
    } while (sz > 0 || !sbufIsPFinished(sb));
    xaFree(buf);

    if (close)
        fsClose(file);

    bool ret = !sbufIsError(sb);
    sbufCFinish(sb);
    return ret;
}

_Use_decl_annotations_
bool sbufFSFileCRegisterPush(StreamBuffer *sb, FSFile *file, bool close)
{
    SbufFSFileCtx *sbc = xaAlloc(sizeof(SbufFSFileCtx));
    sbc->file = file;
    sbc->close = close;

    if (!sbufCRegisterPush(sb, sbufFSFileNotifyCB, sbufFSFileCleanup, sbc)) {
        sbufFSFileCleanup(sbc);
        return false;
    }

    return true;
}
