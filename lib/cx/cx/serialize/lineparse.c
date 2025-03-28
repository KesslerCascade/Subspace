#include "lineparse.h"
#include <cx/cx.h>
#include <cx/string.h>
#include <cx/utils/compare.h>

#define LPCHUNK 128

typedef struct LineParserContext {
    uint32 flags;

    size_t checked;     // buffer offset that has already been checked for EOL
    string out;         // cached output string

    lparseLineCB lineCB;
    void *userCtx;
    sbufCleanupCB userCleanupCB;
} LineParserContext;

typedef struct EOLFindInfo
{
    size_t off;
    int len;
} EOLFindInfo;

static bool findEOLLF(_Inout_ EOLFindInfo *ei, _In_reads_bytes_(sz) const uint8 *buf, size_t sz, _Inout_ LineParserContext *lpc)
{
    for (size_t i = 0; i < sz; i++) {
        if (buf[i] == '\n') {
            ei->off = i;
            ei->len = 1;
            return true;
        }
    }
    return false;
}

static bool findEOLCRLF(_Inout_ EOLFindInfo *ei, _In_reads_bytes_(sz) const uint8 *buf, size_t sz, _Inout_ LineParserContext *lpc)
{
    for (size_t i = 0; i + 1 < sz; i++) {
        if (buf[i] == '\r' && buf[i + 1] == '\n') {
            ei->off = i;
            ei->len = 2;
            return true;
        }
    }
    return false;
}

static bool findEOLMixed(_Inout_ EOLFindInfo *ei, _In_reads_bytes_(sz) const uint8 *buf, size_t sz, _Inout_ LineParserContext *lpc)
{
    for (size_t i = 0; i < sz; i++) {
        if (i + 1 < sz && buf[i] == '\r' && buf[i + 1] == '\n') {
            ei->off = i;
            ei->len = 2;
            return true;
        } else if (buf[i] == '\n') {
            ei->off = i;
            ei->len = 1;
            return true;
        }
    }
    return false;
}

static bool findEOLAuto(_Inout_ EOLFindInfo *ei, _In_reads_bytes_(sz) const uint8 *buf, size_t sz, _Inout_ LineParserContext *lpc)
{
    for (size_t i = 0; i < sz; i++) {
        if (i + 1 < sz && buf[i] == '\r' && buf[i + 1] == '\n') {
            ei->off = i;
            ei->len = 2;
            lpc->flags |= LPARSE_CRLF;
            return true;
        } else if (buf[i] == '\n') {
            ei->off = i;
            ei->len = 1;
            lpc->flags |= LPARSE_LF;
            return true;
        }
    }
    return false;
}

// these must be kept in the same order as the flags 0-3 in LINEPARSER_FLAGS_ENUM!
static bool (*eolfuncs[])(_Inout_ EOLFindInfo *ei, _In_reads_bytes_(sz) const uint8 *buf, size_t sz, _Inout_ LineParserContext *lpc) = {
    findEOLAuto,
    findEOLCRLF,
    findEOLLF,
    findEOLMixed
};

_Static_assert((sizeof(eolfuncs) / sizeof(eolfuncs[0]) == LPARSE_EOL_COUNT), "Wrong number of EOL functions");

static void lpcCleanup(_Pre_valid_ void *ctx)
{
    LineParserContext *lpc = (LineParserContext *)ctx;

    if (lpc->userCleanupCB)
        lpc->userCleanupCB(lpc->userCtx);

    strDestroy(&lpc->out);

    xaFree(lpc);
}

_Use_decl_annotations_
bool lparseRegisterPull(StreamBuffer *sb, uint32 flags)
{
    LineParserContext *lpc = xaAlloc(sizeof(LineParserContext), XA_Zero);

    if (!sbufCRegisterPull(sb, lpcCleanup, lpc))
        return false;

    lpc->flags = flags;

    return true;
}

// Returns false when there are no more lines.
_Use_decl_annotations_
bool lparseLine(StreamBuffer *sb, string *out)
{
    LineParserContext *lpc = (LineParserContext *)sb->consumerCtx;
    EOLFindInfo ei;
    uint8 buf[LPCHUNK];
    uint8 *outbuf;
    size_t didread;

    strClear(out);

    for(;;) {
        // feed buffer if we have already checked everything
        if (sbufCAvail(sb) - lpc->checked <= 1) {
            // check for <= 1 because of the 1-byte overlap mentioned below
            sbufCFeed(sb, lpc->checked + LPCHUNK);
        }

        size_t tocheck = min(sbufCAvail(sb) - lpc->checked, LPCHUNK);
        if (!sbufCPeek(sb, buf, lpc->checked, tocheck))
            break;

        if (eolfuncs[lpc->flags & LPARSE_EOL_MASK](&ei, buf, tocheck, lpc)) {
            // adjust offset since we only started looking at lpc->checked
            ei.off += lpc->checked;

            // found one!
            if (!(lpc->flags & LPARSE_IncludeEOL)) {
                // EOL in string (default)
                outbuf = strBuffer(out, (uint32)ei.off);
                sbufCRead(sb, outbuf, (uint32)ei.off, &didread);
                devAssert(didread == ei.off);
                sbufCSkip(sb, ei.len);
            } else {
                // include EOL character(s) in string
                outbuf = strBuffer(out, (uint32)ei.off + ei.len);
                sbufCRead(sb, outbuf, ei.off + ei.len, &didread);
                devAssert(didread == ei.off + ei.len);
            }
            // buffer has been advanced to right after the EOL
            lpc->checked = 0;
            return true;
        }

        if (sbufIsPFinished(sb)) {
            // no EOL but we have everything we're going to get
            if (sbufCAvail(sb) > 0 && !(lpc->flags & LPARSE_NoIncomplete)) {
                outbuf = strBuffer(out, (uint32)sbufCAvail(sb));
                sbufCRead(sb, outbuf, sbufCAvail(sb), &didread);
                return true;
            }

            sbufCFinish(sb);
            return false;
        }

        // Mark that we've already checked this part,
        // but offset it so there's a 1-byte overlap.
        // This is to catch CRLF pairs that get split across buffer boundaries.
        lpc->checked += max(tocheck, 1) - 1;
    }

    return false;
}

// -------- Push mode --------

static void lpcNotify(_Pre_valid_ StreamBuffer *sb, size_t sz, _Pre_opt_valid_ void *ctx)
{
    LineParserContext *lpc = (LineParserContext *)ctx;
    EOLFindInfo ei;
    uint8 buf[LPCHUNK];
    uint8 *outbuf;
    size_t didread;

    // keep looking for lines as long as there's data in the buffer
    while (sbufCAvail(sb) > 0 && lpc->checked < sbufCAvail(sb) - (sbufIsPFinished(sb) ? 0 : 1)) {
        size_t tocheck = min(sbufCAvail(sb) - lpc->checked, LPCHUNK);
        if (!sbufCPeek(sb, buf, lpc->checked, tocheck))
            break;

        if (eolfuncs[lpc->flags & LPARSE_EOL_MASK](&ei, buf, tocheck, lpc)) {
            // adjust offset since we only started looking at lpc->checked
            ei.off += lpc->checked;

            // found one!
            strClear(&lpc->out);
            if (!(lpc->flags & LPARSE_IncludeEOL)) {
                // EOL in string (default)
                outbuf = strBuffer(&lpc->out, (uint32)ei.off);
                sbufCRead(sb, outbuf, (uint32)ei.off, &didread);
                devAssert(didread == ei.off);
                sbufCSkip(sb, ei.len);
            } else {
                // include EOL character(s) in string
                outbuf = strBuffer(&lpc->out, (uint32)ei.off + ei.len);
                sbufCRead(sb, outbuf, ei.off + ei.len, &didread);
                devAssert(didread == ei.off + ei.len);
            }
            // buffer has been advanced to right after the EOL
            lpc->checked = 0;

            if (!lpc->lineCB(lpc->out, lpc->userCtx)) {
                sbufCFinish(sb);
                return;
            }
        } else {
            lpc->checked += tocheck - (sbufIsPFinished(sb) ? 0 : 1);
        }
    }

    if (sbufIsPFinished(sb)) {
        if (sbufCAvail(sb) > 0 && !(lpc->flags & LPARSE_NoIncomplete)) {
            // no EOL but we have everything we're going to get
            strClear(&lpc->out);
            outbuf = strBuffer(&lpc->out, (uint32)sbufCAvail(sb));
            sbufCRead(sb, outbuf, sbufCAvail(sb), &didread);
            lpc->lineCB(lpc->out, lpc->userCtx);
        }

        sbufCFinish(sb);
    }
}

_Use_decl_annotations_
bool lparseRegisterPush(StreamBuffer *sb, lparseLineCB pline, sbufCleanupCB pcleanup, void *ctx, uint32 flags)
{
    LineParserContext *lpc = xaAlloc(sizeof(LineParserContext), XA_Zero);

    lpc->userCtx = ctx;
    lpc->userCleanupCB = pcleanup;

    if (!pline || !sbufCRegisterPush(sb, lpcNotify, lpcCleanup, lpc))
        return false;

    lpc->lineCB = pline;
    lpc->flags = flags;

    return true;
}
