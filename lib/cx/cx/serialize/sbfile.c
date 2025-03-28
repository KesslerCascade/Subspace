#include "sbfile.h"

typedef struct SbufFileCtx {
    VFSFile *file;
    bool close;
} SbufFileCtx;

static void sbufFileCleanup(_Pre_valid_ void *ctx)
{
    SbufFileCtx *sbc = (SbufFileCtx *)ctx;
    if (sbc->close)
        vfsClose(sbc->file);
    xaFree(sbc);
}

_Use_decl_annotations_
bool sbufFileIn(StreamBuffer *sb, VFSFile *file, bool close)
{
    if (!sbufPRegisterPush(sb, NULL, NULL))
        return false;

    uint8 *buf = xaAlloc(sb->targetsz);
    size_t didread = 0;
    for(;;) {
        if (!vfsRead(file, buf, sb->targetsz, &didread)) {
            sbufError(sb);
            break;
        }

        if (didread == 0)           // EOF
            break;

        sbufPWrite(sb, buf, didread);
    }
    xaFree(buf);

    if (close)
        vfsClose(file);

    bool ret = !sbufIsError(sb);
    sbufPFinish(sb);
    return ret;
}

static size_t sbufFilePullCB(_Pre_valid_ StreamBuffer *sb, _Out_writes_bytes_(sz) uint8 *buf, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufFileCtx *sbc = (SbufFileCtx *)ctx;
    if (!sbc)
        return 0;

    size_t didread = 0;
    if (!vfsRead(sbc->file, buf, sz, &didread))
        sbufError(sb);

    if (didread == 0)
        sbufPFinish(sb);

    return didread;
}

_Use_decl_annotations_
bool sbufFilePRegisterPull(StreamBuffer *sb, VFSFile *file, bool close)
{
    SbufFileCtx *sbc = xaAlloc(sizeof(SbufFileCtx));
    sbc->file = file;
    sbc->close = close;

    if (!sbufPRegisterPull(sb, sbufFilePullCB, sbufFileCleanup, sbc)) {
        sbufFileCleanup(sbc);
        return false;
    }

    return true;
}

static bool sbufFileSendCB(_Pre_valid_ StreamBuffer *sb, _In_reads_bytes_(sz) const uint8 *buf, size_t off, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufFileCtx *sbc = (SbufFileCtx *)ctx;
    if (!sbc)
        return false;

    size_t didwrite = 0;
    if (!vfsWrite(sbc->file, (void*)buf, sz, &didwrite))
        sbufError(sb);

    return true;
}

static void sbufFileNotifyCB(_Pre_valid_ StreamBuffer *sb, size_t sz, _Pre_opt_valid_ void *ctx)
{
    if (sz >= (sb->targetsz >> 1) + (sb->targetsz >> 2)) {
        sbufCSend(sb, sbufFileSendCB, sz);
    } else if (sz == 0 || sbufIsPFinished(sb)) {
        // flush anything that's left in the streambuf
        sbufCSend(sb, sbufFileSendCB, sbufCAvail(sb));
    }
}

_Use_decl_annotations_
bool sbufFileOut(StreamBuffer *sb, VFSFile *file, bool close)
{
    if (!sbufCRegisterPull(sb, NULL, NULL))
        return false;

    uint8 *buf = xaAlloc(sb->targetsz);
    size_t sz;
    do {
        // grab targetsz at a time from the buffer
        if (sbufCRead(sb, buf, sb->targetsz, &sz)) {
            size_t didwrite;
            if (!vfsWrite(file, buf, sz, &didwrite)) {
                sbufError(sb);
                break;
            }
        }
    } while (sz > 0 || !sbufIsPFinished(sb));
    xaFree(buf);

    if (close)
        vfsClose(file);

    bool ret = !sbufIsError(sb);
    sbufCFinish(sb);
    return ret;
}

_Use_decl_annotations_
bool sbufFileCRegisterPush(StreamBuffer *sb, VFSFile *file, bool close)
{
    SbufFileCtx *sbc = xaAlloc(sizeof(SbufFileCtx));
    sbc->file = file;
    sbc->close = close;

    if (!sbufCRegisterPush(sb, sbufFileNotifyCB, sbufFileCleanup, sbc)) {
        sbufFileCleanup(sbc);
        return false;
    }

    return true;
}
