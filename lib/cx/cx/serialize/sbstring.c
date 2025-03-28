#include "sbstring.h"
#include <cx/string.h>
#include <cx/utils/compare.h>

typedef struct SbufStrInCtx {
    striter iter;
} SbufStrInCtx;

static void sbufStrInCleanup(_Pre_opt_valid_ void *ctx)
{
    if (!ctx)
        return;

    SbufStrInCtx *sbc = (SbufStrInCtx *)ctx;
    striFinish(&sbc->iter);
    xaFree(sbc);
}

_Use_decl_annotations_
bool sbufStrIn(StreamBuffer *sb, strref str)
{
    striter si;

    if (!sbufPRegisterPush(sb, NULL, NULL))
        return false;

    striBorrow(&si, str);

    // iterate entire string
    while (si.len > 0) {
        while (si.cursor < si.len) {
            // push at most the target buffer size
            uint32 nbytes = min(si.len - si.cursor, (uint32)sb->targetsz);
            sbufPWrite(sb, si.bytes + si.cursor, nbytes);
            si.cursor += nbytes;
        }
        striNext(&si);
    }

    striFinish(&si);
    bool ret = !sbufIsError(sb);
    sbufPFinish(sb);
    return ret;
}

static size_t sbufStrPullCB(_Pre_valid_ StreamBuffer *sb, _Out_writes_bytes_(sz) uint8 *buf, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufStrInCtx *sbc = (SbufStrInCtx *)ctx;
    if (!sbc)
        return 0;

    uint32 nbytes = min(sbc->iter.len - sbc->iter.cursor, (uint32)sz);
    if (nbytes > 0) {
        memcpy(buf, sbc->iter.bytes + sbc->iter.cursor, nbytes);
        striAdvance(&sbc->iter, nbytes);
    }

    if (sbc->iter.len == 0)
        sbufPFinish(sb);

    return nbytes;
}

_Use_decl_annotations_
bool sbufStrPRegisterPull(StreamBuffer *sb, strref str)
{
    SbufStrInCtx *sbc = xaAlloc(sizeof(SbufStrInCtx));

    striInit(&sbc->iter, str);

    if (!sbufPRegisterPull(sb, sbufStrPullCB, sbufStrInCleanup, sbc))
        return false;

    return true;
}

typedef struct SbufStrOutCtx {
    string *out;
} SbufStrOutCtx;

static void sbufStrOutCleanup(_Pre_opt_valid_ void *ctx)
{
    SbufStrOutCtx *sbc = (SbufStrOutCtx *)ctx;
    xaFree(sbc);
}

static void sbufStrNotifyCB(_Pre_valid_ StreamBuffer *sb, size_t sz, _Pre_opt_valid_ void *ctx)
{
    SbufStrOutCtx *sbc = (SbufStrOutCtx *)ctx;
    if (!sbc)
        return;

    string temp = 0;
    uint8 *tbuf = strBuffer(&temp, (uint32)sz);
    if (sbufCRead(sb, tbuf, sz, &sz)) {
        strSetLen(&temp, (uint32)sz);
        strAppend(sbc->out, temp);
    }
    strDestroy(&temp);
}

_Use_decl_annotations_
bool sbufStrOut(StreamBuffer *sb, string *strout)
{
    if (!sbufCRegisterPull(sb, NULL, NULL))
        return false;

    string temp = 0;
    size_t sz;
    do {
        // grab targetsz at a time from the buffer
        strClear(&temp);
        uint8 *tbuf = strBuffer(&temp, (uint32)sb->targetsz);

        if (sbufCRead(sb, tbuf, sb->targetsz, &sz)) {
            strSetLen(&temp, (uint32)sz);
            strAppend(strout, temp);
        }
    } while (sz > 0 || !sbufIsPFinished(sb));

    strDestroy(&temp);

    bool ret = !sbufIsError(sb);
    sbufCFinish(sb);
    return ret;
}

_Use_decl_annotations_
bool sbufStrCRegisterPush(StreamBuffer *sb, string *strout)
{
    SbufStrOutCtx *sbc = xaAlloc(sizeof(SbufStrOutCtx));
    sbc->out = strout;

    if (!sbufCRegisterPush(sb, sbufStrNotifyCB, sbufStrOutCleanup, sbc))
        return false;

    return true;
}

_Use_decl_annotations_
StreamBuffer *sbufStrCreatePush(string *strout, size_t targetsz)
{
    StreamBuffer* ret = sbufCreate(targetsz);
    if (!ret)
        return NULL;

    if(!sbufPRegisterPush(ret, NULL, NULL)) {
        sbufRelease(&ret);
        return NULL;
    }

    if(!sbufStrCRegisterPush(ret, strout)) {
        sbufRelease(&ret);
        return NULL;
    }

    return ret;
}