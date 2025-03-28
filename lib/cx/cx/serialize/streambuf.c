#include "streambuf.h"
#include <cx/container/foreach.h>
#include <cx/debug/error.h>
#include <cx/meta/block.h>
#include <cx/string/striter.h>
#include <cx/utils/compare.h>

static void sbufPFinishInternal(_Inout_ StreamBuffer *sb);
static void sbufCFinishInternal(_Inout_ StreamBuffer *sb);

_Use_decl_annotations_
StreamBuffer *sbufCreate(size_t targetsz)
{
    StreamBuffer *ret = xaAlloc(sizeof(StreamBuffer), XA_Zero);

    ret->refcount = 1;

    if (targetsz > 0) {
        // minimum is just to keep the math from blowing up, it's very low for unit tests
        ret->targetsz = max(targetsz, 8);
        ret->bufsz = ret->targetsz / 4;
        ret->buf = xaAlloc(ret->bufsz);
    } else {
        // targetsz == 0 is used only for direct mode,
        // go ahead and lock this buffer into push mode
        ret->flags |= SBUF_Push;
    }

    return ret;
}

static void sbufDestroy(_Pre_valid_ _Post_invalid_ StreamBuffer *sb)
{
    if (sb->consumerCleanup)
        sb->consumerCleanup(sb->consumerCtx);
    if (sb->producerCleanup)
        sb->producerCleanup(sb->producerCtx);

    xaFree(sb->buf);
    xaFree(sb->overflow);

    xaFree(sb);
}

_Use_decl_annotations_
void sbufRelease(StreamBuffer **sb)
{
    if (*sb) {
        (*sb)->refcount--;
        if ((*sb)->refcount <= 0)
            sbufDestroy(*sb);
        (*sb) = NULL;
    }
}

_Use_decl_annotations_
void sbufError(StreamBuffer *sb)
{
    sb->flags |= SBUF_Error;

    if (!(sb->flags & SBUF_Consumer_Done)) {
        if (sb->consumerNotify)
            sb->consumerNotify(sb, 0, sb->consumerCtx);
        if (sb->consumerPush)
            sb->consumerPush(sb, NULL, 0, sb->consumerCtx);
    }

    if (!(sb->flags & SBUF_Producer_Done) && sb->producerPull)
        sb->producerPull(sb, NULL, 0, sb->producerCtx);
}

_Use_decl_annotations_
bool sbufPRegisterPull(StreamBuffer *sb, sbufPullCB ppull, sbufCleanupCB pcleanup, void *ctx)
{
    // can only register once, if it's not already a push buffer
    if (sbufIsPush(sb) ||
        (sb->flags & SBUF_Producer_Registered) ||
        !ppull) {
        if (pcleanup)
            pcleanup(ctx);

        cxerr = CX_InvalidArgument;
        return false;
    }  

    sb->producerPull = ppull;
    sb->producerCleanup = pcleanup;
    sb->producerCtx = ctx;
    sb->flags |= SBUF_Producer_Registered | SBUF_Pull;
    sb->refcount++;

    return true;
}

_Use_decl_annotations_
bool sbufPRegisterPush(StreamBuffer *sb, sbufCleanupCB pcleanup, void *ctx)
{
    // can only register once, if it's not already a pull buffer
    if (sbufIsPull(sb) ||
        (sb->flags & SBUF_Producer_Registered)) {
        if (pcleanup)
            pcleanup(ctx);

        cxerr = CX_InvalidArgument;
        return false;
    }

    sb->producerCleanup = pcleanup;
    sb->producerCtx = ctx;
    sb->flags |= SBUF_Producer_Registered | SBUF_Push;
    sb->refcount++;

    return true;
}

_Use_decl_annotations_
size_t sbufPAvail(StreamBuffer *sb)
{
    if (sb->overflowsz > 0) {
        // we're writing to the overflow buffer
        return sb->overflowsz - sb->overflowtail - 1;
    } else if (sb->head <= sb->tail) {
        // simple buffer
        // remaining space is the space left to the end of the buffer, plus any gap at the beginning,
        // less 1 because head == tail is empty not full
        return sb->bufsz - sb->tail + sb->head - 1;
    } else {
        // wraparound buffer
        // remaining space is the gap between the tail and the head,
        // less 1 because head == tail is empty not full
        return sb->head - sb->tail - 1;
    }
}

_Use_decl_annotations_
size_t sbufCAvail(StreamBuffer *sb)
{
    if (sb->head <= sb->tail) {
        // simple buffer
        // available data is anything between head and tail, plus whatever's in overflow
        return sb->tail - sb->head + sb->overflowtail;
    } else {
        // wraparound buffer
        // available data is data from head to end, then to tail, plus whatever's in overflow
        // less 1 because head == tail is empty not full
        return sb->bufsz - sb->head + sb->tail + sb->overflowtail;
    }
}

static size_t adjustSize(size_t startsz, size_t needed, size_t targetsz)
{
    // size the buffer for 1 byte larger than needed because actual capacity
    // loses a byte due to head == tail meaning empty.
    while (startsz < needed + 1) {
        startsz += startsz >> 1;
    }

    // try not to go over target if possible
    if (startsz > targetsz && needed + 1 <= targetsz)
        startsz = targetsz;

    return startsz;
}

static void pushMakeBufSpace(_Inout_ StreamBuffer *sb, size_t sz)
{
    // check if there's space
    if (sb->overflowsz > 0) {
        // if we already have an overflow buffer, increase it if necessary
        if (sz > sb->overflowsz - sb->overflowtail - 1) {
            sb->overflowsz = adjustSize(sb->overflowsz, sb->overflowtail + sz, sb->targetsz);
            xaResize(&sb->overflow, sb->overflowsz);
        }
    } else {
        // no overflow, check regular buffer size
        if (sbufPAvail(sb) < sz) {
            // not enough room, make some
            if (sb->head == sb->tail) {
                // buffer is empty, just reallocate it
                xaFree(sb->buf);
                sb->bufsz = adjustSize(sb->bufsz, sz, sb->targetsz);
                sb->buf = xaAlloc(sb->bufsz);
            } else {
                // there's data in the buffer, need to go to overflow
                // grow buffer if below target since it will replace the current buffer
                sb->overflowsz = adjustSize(sb->bufsz + (sb->bufsz >> 1), sz, sb->targetsz);
                sb->overflowtail = 0;
                sb->overflow = xaAlloc(sb->overflowsz);
            }
        }
    }
}

_Use_decl_annotations_
bool sbufPWrite(StreamBuffer *sb, const uint8 *buf, size_t sz)
{
    if (sz == 0)
        return true;

    devAssert(sb->flags & SBUF_Producer_Registered);
    devAssert(!(sb->flags & SBUF_Producer_Done));
    devAssert(sb->flags & SBUF_Consumer_Registered);

    if (sbufIsCFinished(sb)) {
        // nobody's listening
        return false;
    }

    if (sb->flags & SBUF_Direct) {
        // for direct mode just call the callback
        sb->consumerPush(sb, buf, sz, sb->consumerCtx);
    } else {
        // normal (notify) mode
        pushMakeBufSpace(sb, sz);

        if (sb->overflowsz == 0) {
            // writing to normal buffer
            if (sb->tail + sz < sb->bufsz) {
                // simple write
                memcpy(sb->buf + sb->tail, buf, sz);
                sb->tail = (sb->tail + sz) % sb->bufsz;     // catch edge case where we end right at bufsz
            } else {
                // wraparound write
                size_t segment = sb->bufsz - sb->tail;
                memcpy(sb->buf + sb->tail, buf, segment);
                memcpy(sb->buf, buf + segment, sz - segment);
                sb->tail = (sz - segment) % sb->bufsz;
            }
        } else {
            // writing to overflow buffer
            devAssert(sb->overflowtail + sz < sb->overflowsz);
            memcpy(sb->overflow + sb->overflowtail, buf, sz);
            sb->overflowtail += sz;
        }

        // Notify consumer there's data in the buffer, but only in push mode.
        // This function may also be used in pull mode if the callback doesn't have
        // enough buffer space to write what it wants.
        if (sbufIsPush(sb)) {
            sb->consumerNotify(sb, sbufCAvail(sb), sb->consumerCtx);
        }
    }

    return true;
}

_Use_decl_annotations_
bool sbufPWriteStr(StreamBuffer *sb, strref str)
{
    foreach(string, it, str)
    {
        if (!sbufPWrite(sb, it.bytes, it.len))
            return false;
    }
    return true;
}

_Use_decl_annotations_
bool sbufPWriteLine(StreamBuffer *sb, strref str)
{
    foreach (string, it, str) {
        if (!sbufPWrite(sb, it.bytes, it.len))
            return false;
    }
#ifdef _PLATFORM_WIN
    return sbufPWrite(sb, (const uint8*)"\r\n", 2);
#else
    return sbufPWrite(sb, (const uint8*)"\n", 1);
#endif
    return true;
}

_Use_decl_annotations_
bool sbufPWriteEOL(StreamBuffer* sb)
{
#ifdef _PLATFORM_WIN
    return sbufPWrite(sb, (const uint8*)"\r\n", 2);
#else
    return sbufPWrite(sb, (const uint8*)"\n", 1);
#endif
    return true;
}

_Use_decl_annotations_
static void sbufPFinishInternal(StreamBuffer *sb)
{
    if (sb->flags & SBUF_Producer_Done)
        return;

    sb->flags |= SBUF_Producer_Done;

    // notify consumer about EOF
    if (!(sb->flags & SBUF_Consumer_Done)) {
        if (sb->consumerNotify) {
            // notify once for any remaining data in buffer, then again for EOF
            size_t left = sbufCAvail(sb);
            if (left > 0)
                sb->consumerNotify(sb, left, sb->consumerCtx);

            // check flag again in case they finished in the previous callback
            if (!(sb->flags & SBUF_Consumer_Done))
                sb->consumerNotify(sb, 0, sb->consumerCtx);
        } else if (sb->consumerPush)
            sb->consumerPush(sb, NULL, 0, sb->consumerCtx);
    }

    // if buffer is in push mode, consumer has gotten all the callbacks they're going to get
    if (sbufIsPush(sb))
        sbufCFinishInternal(sb);

    sbufRelease(&sb);
}

_Use_decl_annotations_
void sbufPFinish(StreamBuffer *sb)
{
    sbufPFinishInternal(sb);
}

_Use_decl_annotations_
bool sbufCRegisterPull(StreamBuffer *sb, sbufCleanupCB ccleanup, void *ctx)
{
    // can only register once, if it's not already a push buffer
    if (sbufIsPush(sb) ||
        (sb->flags & SBUF_Consumer_Registered)) {
        if (ccleanup)
            ccleanup(ctx);

        cxerr = CX_InvalidArgument;
        return false;
    }

    sb->consumerCleanup = ccleanup;
    sb->consumerCtx = ctx;
    sb->flags |= SBUF_Consumer_Registered | SBUF_Pull;
    sb->refcount++;

    return true;
}

_Use_decl_annotations_
bool sbufCRegisterPush(StreamBuffer *sb, sbufNotifyCB cnotify, sbufCleanupCB ccleanup, void *ctx)
{
    // can only register once, if it's not already a pull buffer, and is not set as a direct buffer
    if (sbufIsPull(sb) ||
        (sb->flags & SBUF_Consumer_Registered) ||
        sb->bufsz == 0 ||
        !cnotify) {
        if (ccleanup)
            ccleanup(ctx);

        cxerr = CX_InvalidArgument;
        return false;
    }

    sb->consumerNotify = cnotify;
    sb->consumerCleanup = ccleanup;
    sb->consumerCtx = ctx;
    sb->flags |= SBUF_Consumer_Registered | SBUF_Push;
    sb->refcount++;

    return true;
}

_Use_decl_annotations_
bool sbufCRegisterPushDirect(StreamBuffer *sb, sbufPushCB cpush, sbufCleanupCB ccleanup, void *ctx)
{
    // can only register once, if it's not already a pull buffer, and is not set as a direct buffer
    if (sbufIsPull(sb) ||
        (sb->flags & SBUF_Consumer_Registered) ||
        !cpush) {
        if (ccleanup)
            ccleanup(ctx);

        cxerr = CX_InvalidArgument;
        return false;
    }

    sb->consumerPush = cpush;
    sb->consumerCleanup = ccleanup;
    sb->consumerCtx = ctx;
    sb->flags |= SBUF_Consumer_Registered | SBUF_Push | SBUF_Direct;
    sb->refcount++;

    return true;
}

static inline void _readWriteBuf(_Inout_ StreamBuffer *sb, _Out_writes_bytes_opt_(sz) uint8 *out, _Inout_ size_t *p, _Inout_ size_t *total, _In_reads_bytes_(sz) const uint8 *in, _Inout_ size_t *off, size_t sz, _Inout_ size_t *skip, bool movehead, _In_opt_ sbufSendCB sendcb)
{
    size_t skipsz = min(*skip, sz);
    *skip -= skipsz;
    *total -= skipsz;
    *off += skipsz;
    sz -= skipsz;
    if (movehead)
        sb->head = (sb->head + skipsz) % sb->bufsz;

    if (sz > 0) {
        if (out) {
            memcpy(out + *p, in + skipsz, sz);
            if (movehead)
                sb->head = (sb->head + sz) % sb->bufsz;
        } else if (sendcb) {
            if (sendcb(sb, in + skipsz, *off, sz, sb->consumerCtx))
                sb->head = (sb->head + skipsz + sz) % sb->bufsz;        // no real good way to handle skipsz here
        }
        *total -= sz;
        *p += sz;
        *off += sz;
    }

    if(sb->head == sb->tail) {
        // If the buffer is empty, reset it to the start of the ring.
        // This helps avoid having to split reads/writes.
        sb->head = sb->tail = 0;
    }
}
#define readWriteBuf(out, p, in, sz) _readWriteBuf(sb, out, &p, &total, in, &off, sz, &skip, movehead, sendcb)

// caller must verify there's enough data in the buffer first!!!!
static void readCommon(_Inout_ StreamBuffer *sb, _Out_writes_bytes_opt_(skip + bsz) uint8 *buf, size_t skip, size_t bsz, bool movehead, _In_opt_ sbufSendCB sendcb)
{
    // TODO: These don't really all need to be separate variables; some can be redefined as a calculation
    size_t total = skip + bsz;
    size_t count, skipcount;
    size_t p = 0;
    size_t off = 0;

    if (sb->head <= sb->tail) {
        count = min(sb->tail - sb->head, total);
        readWriteBuf(buf, p, sb->buf + sb->head, count);
    } else {
        // wraparound buffer
        count = min(sb->bufsz - sb->head, total);
        readWriteBuf(buf, p, sb->buf + sb->head, count);

        count = min(sb->tail, total);
        if (count > 0) {
            readWriteBuf(buf, p, sb->buf, count);
        }
    }

    // get the rest from overflow
    if (sb->overflowsz > 0) {
        if (total > 0) {
            if (movehead)
                devAssert(sb->head == sb->tail);

            count = min(sb->overflowsz, total);
            skipcount = min(skip, count);
            skip -= skipcount;
            total -= skipcount;
            count -= skipcount;
            off += skipcount;

            if (count > 0) {
                if (buf) {
                    memcpy(buf + p, sb->overflow + skipcount, count);
                    // don't advance the head, that happens when the buffer swaps
                } else if (sendcb) {
                    // if push callback returns false, it doesn't want to consume the overflow data, so set
                    // count to 0. If the buffer rotation happens (because all non-overflow data was consumed),
                    // this ensures that the new head points to the start of the overflow.
                    if (!sendcb(sb, sb->overflow + skipcount, off, count, sb->consumerCtx)) {
                        count = 0;
                        skipcount = 0;
                    }
                }
            }
        } else {
            count = 0;
            skipcount = 0;
        }

        if (sb->head == sb->tail) {
            // overflow buffer becomes main buffer
            xaFree(sb->buf);
            sb->buf = sb->overflow;
            sb->bufsz = sb->overflowsz;
            sb->head = (count + skipcount) % sb->bufsz;
            sb->tail = sb->overflowtail % sb->bufsz;

            sb->overflow = NULL;
            sb->overflowsz = 0;
            sb->overflowtail = 0;
        }
        total -= count;
        p += count;
        off += count;
    }

    devAssert(total == 0);
}

static void feedBuffer(_Inout_ StreamBuffer *sb, size_t want)
{
    size_t needed = want - sbufCAvail(sb);
    size_t count;
    if (sb->head <= sb->tail && sb->tail < sb->bufsz - (sb->head == 0 ? 1 : 0)) {
        // writing at the end of a buffer
        count = sb->producerPull(sb, sb->buf + sb->tail,
                                 sb->bufsz - sb->tail - (sb->head == 0 ? 1 : 0),    // fill as much of the buffer as we can
                                 sb->producerCtx);
        sb->tail = (sb->tail + count) % sb->bufsz;
        return;             // ensure we only ever do one thing per cycle
    } else if (sb->head > sb->tail && sb->tail < sb->head - 1) {
        // writing to the start of the buffer
        count = sb->producerPull(sb, sb->buf + sb->tail,
                                 sb->head - sb->tail - 1,       // fill as much of the buffer as we can
                                 sb->producerCtx);
        sb->tail = (sb->tail + count) % sb->bufsz;
        return;
    } else {
        // off to overflow!
        if (sb->overflowsz - sb->overflowtail < needed + 1) {
            if (sb->overflowsz == 0) {
                sb->overflowsz = adjustSize(sb->bufsz + (sb->bufsz >> 1), needed, sb->targetsz);
                sb->overflowtail = 0;
                sb->overflow = xaAlloc(sb->overflowsz);
            } else {
                sb->overflowsz = adjustSize(sb->overflowsz, sb->overflowtail + needed, sb->targetsz);
                xaResize(&sb->overflow, sb->overflowsz);
            }
        }
        count = sb->producerPull(sb, sb->overflow + sb->overflowtail,
                                 sb->overflowsz - sb->overflowtail - 1, // fill as much of the buffer as we can
                                 sb->producerCtx);
        sb->overflowtail += count;
    }
}

_Use_decl_annotations_
bool sbufCRead(StreamBuffer *sb, uint8 *buf, size_t sz, size_t *bytesread)
{
    if ((sb->flags & SBUF_Direct) || sbufIsError(sb) || sz == 0) {
        *bytesread = 0;
        return false;               // can't pull in direct mode!
    }

    if (sbufIsPull(sb)) {
        // loop until we have enough data to satisfy the request
        while (!sbufIsPFinished(sb) && sz > sbufCAvail(sb)) {
            feedBuffer(sb, sz);
        }
        // cap sz at actual data available, which happens on EOF or error
        sz = min(sz, sbufCAvail(sb));
    } else if (sz > sbufCAvail(sb)) {
        // we don't have enough!
        return false;
    }

    readCommon(sb, buf, 0, sz, true, NULL);
    *bytesread = sz;
    return (sz > 0);
}

_Use_decl_annotations_
bool sbufCPeek(StreamBuffer *sb, uint8 *buf, size_t off, size_t sz)
{
    if ((sb->flags & SBUF_Direct) || sbufIsError(sb))
        return false;               // can't peek in direct mode!

    if (sz == 0)
        return true;

    if (off + sz > sbufCAvail(sb)) {
        // we don't have enough!
        return false;
    }

    readCommon(sb, buf, off, sz, false, NULL);

    return true;
}

_Use_decl_annotations_
bool sbufCFeed(StreamBuffer *sb, size_t minsz)
{
    if (!sbufIsPull(sb))
        return false;

    // loop until we have enough data to satisfy the request
    while (!sbufIsPFinished(sb) && minsz > sbufCAvail(sb)) {
        feedBuffer(sb, minsz);
    }

    return sbufCAvail(sb) >= minsz;
}

_Use_decl_annotations_
bool sbufCSkip(StreamBuffer *sb, size_t bytes)
{
    if ((sb->flags & SBUF_Direct) || sbufIsError(sb))
        return false;               // can't seek in direct mode!

    if (bytes == 0)
        return true;

    size_t count;
    if (bytes > sbufCAvail(sb)) {
        // we don't have enough!
        return false;
    }

    // simplified version of readCommon that just advances the head
    if (sb->head <= sb->tail) {
        count = min(sb->tail - sb->head, bytes);
        sb->head += count;
        bytes -= count;
    } else {
        // wraparound buffer
        count = min(sb->bufsz - sb->head, bytes);
        sb->head = (sb->head + count) % sb->bufsz;
        bytes -= count;

        count = min(sb->tail, bytes);
        if (count > 0) {
            sb->head = count % sb->bufsz;
            bytes -= count;
        }
    }

    // get the rest from overflow
    if (sb->overflowsz > 0 && bytes > 0) {
        devAssert(sb->head == sb->tail);

        count = min(sb->overflowsz, bytes);

        // overflow buffer becomes main buffer
        xaFree(sb->buf);
        sb->buf = sb->overflow;
        sb->bufsz = sb->overflowsz;
        sb->head = count % sb->bufsz;
        sb->tail = sb->overflowtail % sb->bufsz;

        sb->overflow = NULL;
        sb->overflowsz = 0;
        sb->overflowtail = 0;
        bytes -= count;
    }

    devAssert(bytes == 0);

    return true;
}

_Use_decl_annotations_
bool sbufCSend(StreamBuffer *sb, sbufSendCB func, size_t sz)
{
    if ((sb->flags & SBUF_Direct) || sbufIsError(sb))
        return false;                   // can't pull in direct mode!

    if (sz == 0)
        return true;

    if (sbufIsPull(sb)) {
        // loop until we have enough data to satisfy the request
        while (!sbufIsPFinished(sb) && sz > sbufCAvail(sb)) {
            feedBuffer(sb, sz);
        }
    }

    // cap sz at actual data available
    sz = min(sz, sbufCAvail(sb));

    readCommon(sb, NULL, 0, sz, true, func);

    return true;
}

_Use_decl_annotations_
static void sbufCFinishInternal(StreamBuffer *sb)
{
    if (sb->flags & SBUF_Consumer_Done)
        return;

    sb->flags |= SBUF_Consumer_Done;

    // if buffer is in pull mode, producer has gotten all the callbacks they're going to get
    if (sbufIsPull(sb))
        sbufPFinishInternal(sb);

    sbufRelease(&sb);
}

_Use_decl_annotations_
void sbufCFinish(StreamBuffer *sb)
{
    sbufCFinishInternal(sb);
}
