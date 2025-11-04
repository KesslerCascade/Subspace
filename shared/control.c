#include <cx/stype/stvar.h>
#include <cx/time.h>
#include <cx/xalloc.h>

#include "control.h"
#include "netsocket.h"

#define TCPBUF_SEND 65536
#define TCPBUF_RECV 65536

saDeclare(ControlField);

static const uint8_t syncmagic[4] = { 0x0e, 0x19, 0x01, 0x17 };

static void recvNotify(StreamBuffer* sb, size_t sz, void* ctx)
{
    // do nothing, the main loop handles processing the buffer
}

static void sendNotify(StreamBuffer* sb, size_t sz, void* ctx)
{
    // do nothing, the main loop handles processing the buffer
}

bool sendToSocket(StreamBuffer* sb, const uint8_t* buf, size_t off, size_t sz, void* ctx)
{
    ControlState* cs = (ControlState*)ctx;
    intptr_t bsent   = send(cs->sock, buf, sz, 0);
    cs->lastsent += bsent > 0 ? bsent : 0;
    return false;   // always peek, because send() might short write and we don't want to consume
                    // all the data
}

void controlInit(ControlState* cs, socket_t sock)
{
    cs->sock   = sock;
    cs->closed = false;

    netSetNonblock(cs->sock, true);
    netSetBuffers(cs->sock, TCPBUF_SEND, TCPBUF_RECV);

    // init buffers, but only once, since this function can also be used to link a new socket to the
    // control state after reconnecting
    if (!cs->init) {
        cs->init    = true;
        // just some temporary space to avoid reallocating a lot
        cs->tmprecv = xaAlloc(TCPBUF_RECV);

        cs->recvbuf = sbufCreate(TCPBUF_RECV * 4);
        cs->sendbuf = sbufCreate(TCPBUF_SEND * 4);

        sbufPRegisterPush(cs->recvbuf, NULL, cs);
        sbufCRegisterPush(cs->recvbuf, recvNotify, NULL, cs);

        sbufPRegisterPush(cs->sendbuf, NULL, cs);
        sbufCRegisterPush(cs->sendbuf, sendNotify, NULL, cs);
    }
}

bool controlSendBuffer(ControlState* cs)
{
    bool ret         = false;
    StreamBuffer* sb = cs->sendbuf;
    while (sbufCAvail(sb) > 0) {
        size_t tosend = min(sbufCAvail(sb), TCPBUF_SEND / 2);
        bool shortsnd = true;
        cs->lastsent  = 0;
        if (sbufCSend(sb, sendToSocket, tosend) && cs->lastsent > 0) {
            sbufCSkip(sb, cs->lastsent);
            ret = true;

            if (cs->lastsent == tosend)
                shortsnd = false;
            cs->lastsent = 0;
        }

        if (shortsnd)
            break;   // couldn't send all of it, buffer might be full
    }
    return ret;
}

bool controlMsgReady(ControlState* cs)
{
    StreamBuffer* sb = cs->recvbuf;

    for (;;) {
        // skip forward until we find a valid sync sequence
        while (sbufCAvail(sb) >= 4) {
            uint8_t scomp[4];
            if (sbufCPeek(sb, scomp, 0, 4) && !memcmp(syncmagic, scomp, 4))
                break;

            // these are NOT the sync bytes; skip forward and try again
            sbufCSkip(sb, 1);
        }

        if (sbufCAvail(sb) > 8) {
            // check size field in header
            uint32_t sz = 0;
            sbufCPeek(sb, (uint8_t*)&sz, 4, 4);
            if (sz > MAX_CONTROL_MSG || sz < sizeof(ControlMsgHeader)) {
                // the size field is insane, so we just skip past and try to sync up to the next one
                sbufCSkip(sb, 4);
                continue;
            }

            if (sbufCAvail(sb) >= sz)
                return true;   // we have an entire message in the buffer!
        }

        // either we are out of sync or don't have the whole message yet, either way, try to receive
        // something from the socket
        intptr_t recvb = recv(cs->sock, cs->tmprecv, TCPBUF_RECV, 0);
        if (recvb > 0) {
            sbufPWrite(sb, cs->tmprecv, recvb);
        } else {
            int err = netError();
            if (recvb == 0 || (err != EAGAIN && err != EWOULDBLOCK))
                cs->closed = true;
            return false;   // haven't received anything else yet
        }
    }
    return false;
}

static int32_t writeOneVal(StreamBuffer* sb, int typ, void* v, size_t rawsz)
{
    int32_t ret = 0;

    switch (typ) {
    case CF_INT:
    case CF_FLOAT32:
        sbufPWrite(sb, v, 4);
        ret = 4;
        break;
    case CF_INT64:
    case CF_FLOAT64:
        sbufPWrite(sb, v, 8);
        ret = 8;
        break;
    case CF_BOOL:
        sbufPWrite(sb, v, 1);
        ret = 1;
        break;
    case CF_STRING: {
        uint32_t len = strLen(*(strref*)v);
        uint16_t ssz = min(len, 65536);   // TODO: Some kind of safety check here
        sbufPWrite(sb, (uint8_t*)&ssz, 2);
        sbufPWriteStr(sb, *(strref*)v);
        ret = len + 2;
        break;
    }
    case CF_RAW:
        sbufPWrite(sb, *(uint8**)v, rawsz);
        ret = rawsz;
        break;
    }

    return ret;
}

// add a message to the output buffer
bool _controlEncodeMsg(ControlState* cs, ControlMsgHeader* hdr, hashtable fields)
{
    // sanity check input type
    if (!(stEq(htKeyType(fields), stType(string)) && stEq(htValType(fields), stType(stvar))))
        return false;

    memcpy(hdr->sync, syncmagic, 4);
    hdr->size  = sizeof(ControlMsgHeader);
    hdr->msgid = cs->nextid++;

    sa_ControlField cfs;
    saInit(&cfs, opaque(ControlField), 4);

    // go through the fields, create the headers, and calculate size
    foreach (hashtable, hti, fields) {
        stvar* val   = htiValPtr(stvar, hti);
        size_t dsize = 0;
        int32 count  = 0;
        int arrtype  = stType(none);

        ControlField ncf = { 0 };
        strCopyOut(htiKey(strref, hti), 0, ncf.h.name, sizeof(ncf.h.name));

        // is this an array?
        if (stvarIs(val, sarray)) {
            count   = saSize(val->data.st_sarray);
            arrtype = stGetId(saElemType(val->data.st_sarray));
            ncf.h.flags |= CF_ARRAY;
        }

        if (arrtype == stTypeId(string) && count > 0) {
            // special case, have to count all the strings, plus lengths
            dsize = 4;
            for (uint32_t j = 0; j < saSize(val->data.st_sarray); j++) {
                sa_string* arr = (sa_string*)&val->data.st_sarray;
                dsize += strLen(arr->a[j]) + 2;
            }
        } else {
            int typeid = (count == 0 ? stGetId(val->type) : arrtype);

            switch (typeid) {
            case stTypeId(int32):
                ncf.h.ftype = CF_INT;
                dsize       = 4;
                break;
            case stTypeId(uint32):
                ncf.h.ftype = CF_INT;
                ncf.h.flags |= CF_UNSIGNED;
                dsize = 4;
                break;
            case stTypeId(float32):
                ncf.h.ftype = CF_FLOAT32;
                dsize       = 4;
                break;
            case stTypeId(int64):
                ncf.h.ftype = CF_INT64;
                dsize       = 8;
                break;
            case stTypeId(uint64):
                ncf.h.ftype = CF_INT64;
                ncf.h.flags |= CF_UNSIGNED;
                dsize = 8;
                break;
            case stTypeId(float64):
                ncf.h.ftype = CF_FLOAT64;
                dsize       = 8;
                break;
            case stTypeId(bool):
                ncf.h.ftype = CF_BOOL;
                dsize       = 1;
                break;
            case stTypeId(string):
                ncf.h.ftype = CF_STRING;
                dsize       = strLen(val->data.st_string) + 2;
                break;
            case stTypeId(opaque):
                ncf.h.ftype = CF_RAW;
                dsize       = stGetSize(val->type);
                break;
            default:
                devFatalError("Unsupported data type for control message field");
            }

            if (count > 0) {
                dsize *= count;
                dsize += 4;
            }
        }

        ncf.h.size = sizeof(ControlFieldHeader) + dsize;
        // align to 4-byte boundary
        ncf.h.size = ((ncf.h.size + 3) / 4) * 4;

        ncf.count = (ncf.h.ftype == CF_RAW) ? dsize : count;
        ncf.d     = *val;
        hdr->size += ncf.h.size;

        saPush(&cfs, opaque, ncf);
    }

    hdr->nfields = saSize(cfs);

    // now write out the message
    StreamBuffer* sb = cs->sendbuf;
    sbufPWrite(sb, (uint8_t*)hdr, sizeof(ControlMsgHeader));

    for (int32 i = 0; i < saSize(cfs); i++) {
        ControlField* cf        = &cfs.a[i];
        ControlFieldHeader* cfh = &cfs.a[i].h;
        int32_t pad             = cfh->size - sizeof(ControlFieldHeader);

        sbufPWrite(sb, (uint8_t*)cfh, sizeof(ControlFieldHeader));
        if (cfh->flags & CF_ARRAY) {
            sbufPWrite(sb, (uint8_t*)&cf->count, 4);
            pad -= 4;
            for (uint32_t j = 0; j < cf->count; j++) {
                switch (cfh->ftype) {
                case CF_INT:
                    pad -= writeOneVal(sb,
                                       cfh->ftype,
                                       &((sa_int32*)&cf->d.data.st_sarray)->a[j],
                                       0);
                    break;
                case CF_INT64:
                    pad -= writeOneVal(sb,
                                       cfh->ftype,
                                       &((sa_int64*)&cf->d.data.st_sarray)->a[j],
                                       0);
                    break;
                case CF_FLOAT32:
                    pad -= writeOneVal(sb,
                                       cfh->ftype,
                                       &((sa_float32*)&cf->d.data.st_sarray)->a[j],
                                       0);
                    break;
                case CF_FLOAT64:
                    pad -= writeOneVal(sb,
                                       cfh->ftype,
                                       &((sa_float64*)&cf->d.data.st_sarray)->a[j],
                                       0);
                    break;
                case CF_BOOL:
                    pad -= writeOneVal(sb, cfh->ftype, &((sa_bool*)&cf->d.data.st_sarray)->a[j], 0);
                    break;
                case CF_STRING:
                    pad -= writeOneVal(sb,
                                       cfh->ftype,
                                       &((sa_string*)&cf->d.data.st_sarray)->a[j],
                                       0);
                    break;
                }
            }
        } else {
            pad -= writeOneVal(sb, cfh->ftype, &cf->d.data, cf->count);
        }

        for (int k = 0; k < pad; k++) {
            uint8_t zero[1] = { 0 };
            sbufPWrite(sb, zero, 1);
        }
    }

    saDestroy(&cfs);

    return true;
}

// decode message parts from a FULLY received message in the buffer (see controlMsgReady)
bool _controlParseHeader(ControlState* cs, ControlMsgHeader* hdr)
{
    if (cs->recvhdr || !controlMsgReady(cs))
        return false;

    StreamBuffer* sb = cs->recvbuf;
    size_t didread;
    sbufCRead(sb, (uint8_t*)hdr, sizeof(ControlMsgHeader), &didread);

    cs->recvhdr = true;
    cs->left    = hdr->size - sizeof(ControlMsgHeader);
    cs->nfields = hdr->nfields;
    cs->nfield  = 0;

    return true;
}

static bool parseField(StreamBuffer* sb, int ftype, void* dest, size_t* pad)
{
    size_t didread;
    switch (ftype) {
    case CF_INT:
    case CF_FLOAT32:
        if (*pad < 4)
            return false;
        sbufCRead(sb, dest, 4, &didread);
        *pad -= 4;
        return true;
    case CF_INT64:
    case CF_FLOAT64:
        if (*pad < 8)
            return false;
        sbufCRead(sb, dest, 8, &didread);
        *pad -= 8;
        return true;
    case CF_BOOL:
        if (*pad < 1)
            return false;
        sbufCRead(sb, dest, 1, &didread);
        *pad -= 1;
        return true;
    case CF_STRING: {
        if (*pad < 2)
            return false;

        uint16_t len = 0;
        sbufCRead(sb, (uint8_t*)&len, 2, &didread);
        *pad -= 2;

        if (*pad < len)
            return false;

        string nstr  = 0;
        uint8_t* buf = strBuffer(&nstr, len);
        sbufCRead(sb, buf, len, &didread);
        *(string*)dest = nstr;

        *pad -= len;
        return true;
    }
    case CF_RAW:
        // read everything that's left in the field
        uint8_t* ptr = xaAlloc(*pad);
        sbufCRead(sb, ptr, *pad, &didread);
        *(void**)dest = ptr;
        *pad          = 0;
        return true;
    }
    return false;
}

bool _controlParseField(ControlState* cs, ControlField* field)
{
    StreamBuffer* sb = cs->recvbuf;
    size_t didread;
    sbufCRead(sb, (uint8_t*)&field->h, sizeof(ControlFieldHeader), &didread);

    ControlFieldHeader* hdr = &field->h;
    size_t pad              = hdr->size - sizeof(ControlFieldHeader);
    memset(&field->d, 0, sizeof(field->d));

    if (hdr->size > MAX_CONTROL_FIELD)
        return false;   // insane

    if (hdr->flags & CF_ARRAY) {
        if (pad < 4)
            return false;
        sbufCRead(sb, (uint8_t*)&field->count, 4, &didread);
        pad -= 4;

        if (field->count > MAX_CONTROL_ARRAY)
            return false;   // insane

        field->d.type = stType(sarray);

        switch (hdr->ftype) {
        case CF_INT:
            if (field->h.flags & CF_UNSIGNED)
                saInit(&field->d.data.st_sarray, uint32, field->count);
            else
                saInit(&field->d.data.st_sarray, int32, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        case CF_INT64:
            if (field->h.flags & CF_UNSIGNED)
                saInit(&field->d.data.st_sarray, uint64, field->count);
            else
                saInit(&field->d.data.st_sarray, int64, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        case CF_FLOAT32:
            saInit(&field->d.data.st_sarray, float32, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        case CF_FLOAT64:
            saInit(&field->d.data.st_sarray, float64, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        case CF_BOOL:
            saInit(&field->d.data.st_sarray, bool, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        case CF_STRING:
            saInit(&field->d.data.st_sarray, string, field->count);
            saSetSize(&field->d.data.st_sarray, field->count);
            break;
        }

        for (uint32_t j = 0; j < field->count; j++) {
            bool success = false;
            switch (hdr->ftype) {
            case CF_INT:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_int32*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            case CF_INT64:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_int64*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            case CF_FLOAT32:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_float32*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            case CF_FLOAT64:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_float64*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            case CF_BOOL:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_bool*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            case CF_STRING:
                success = parseField(sb,
                                     hdr->ftype,
                                     &((sa_string*)&field->d.data.st_sarray)->a[j],
                                     &pad);
                break;
            }
            if (!success)
                return false;
        }
    } else {
        if (hdr->ftype == CF_RAW)
            field->count = pad;   // raw field size of whatever's left

        switch (hdr->ftype) {
        case CF_INT:
            field->d.type = (hdr->flags & CF_UNSIGNED) ? stType(uint32) : stType(int32);
            break;
        case CF_INT64:
            field->d.type = (hdr->flags & CF_UNSIGNED) ? stType(uint64) : stType(int64);
            break;
        case CF_FLOAT32:
            field->d.type = stType(float32);
            break;
        case CF_FLOAT64:
            field->d.type = stType(float64);
            break;
        case CF_BOOL:
            field->d.type = stType(bool);
            break;
        case CF_STRING:
            field->d.type = stType(string);
            break;
        case CF_RAW:
            field->d.type = _stype_mktype(stTypeId(opaque),
                                          stTypeFlags(opaque),
                                          (uint16)field->count);
            break;
        }

        if (!parseField(sb, hdr->ftype, &field->d.data, &pad))
            return false;
    }

    // if there's any padding bytes left, skip them
    if (pad > 0)
        sbufCSkip(sb, pad);

    cs->left -= hdr->size;

    return true;
}

ControlMsg* controlRecvMsg(ControlState* cs)
{
    ControlMsg* ret = xaAllocStruct(ControlMsg, XA_Zero);
    if (!ret)
        goto out;

    if (!_controlParseHeader(cs, &ret->hdr) || ret->hdr.nfields > MAX_CONTROL_ARRAY) {
        xaFree(ret);
        ret = NULL;
        goto out;
    }

    if (ret->hdr.nfields > 0) {
        htInit(&ret->fields, string, stvar, 8);

        for (uint32_t i = 0; i < ret->hdr.nfields; i++) {
            ControlField field = { 0 };
            if (!_controlParseField(cs, &field)) {
                htDestroy(&ret->fields);
                xaFree(ret);
                ret = NULL;
                goto out;
            }

            htInsertC(&ret->fields, strref, (strref)field.h.name, stvar, &field.d);
        }
    }

out:
    _controlParseDone(cs);
    return ret;
}

bool controlSendMsg(ControlState* cs, ControlMsg* msg)
{
    if (!_controlEncodeMsg(cs, &msg->hdr, msg->fields))
        return false;

    return controlSendBuffer(cs);
}

void _controlParseDone(ControlState* cs)
{
    StreamBuffer* sb = cs->recvbuf;
    sbufCSkip(sb, cs->left);
    cs->recvhdr = false;
    cs->left    = 0;
    cs->nfields = 0;
    cs->nfield  = 0;
}

void controlMsgDestroy(ControlMsg* msg)
{
    // special case: fields with an opaque type are owned pointers that need to be freed
    foreach (hashtable, hti, msg->fields) {
        stvar* val = htiValPtr(stvar, hti);
        if (stGetId(val->type) == stTypeId(opaque))
            xaFree(val->data.st_opaque);
    }
    htDestroy(&msg->fields);
    xaFree(msg);
}

ControlMsg* controlMsgCreate(strref cmd)
{
    ControlMsg* ret = xaAllocStruct(ControlMsg, XA_Zero);
#ifdef SUBSPACE_GAME
    // use the current frame time for messages from the game
    ret->hdr.timestamp = osFrameTime();
    ret->priority      = 0;
#else
    ret->hdr.timestamp = clockWall();
#endif

    strCopyOut(cmd, 0, ret->hdr.cmd, sizeof(ret->hdr.cmd));
    htInit(&ret->fields, string, stvar, 8);

    return ret;
}

void controlStateDestroy(ControlState* cs)
{
    // we assume that the socket is closed and abandoned
    if (cs->init) {
        xaFree(cs->tmprecv);

        sbufPFinish(cs->sendbuf);
        sbufCFinish(cs->sendbuf);
        sbufRelease(&cs->sendbuf);

        sbufPFinish(cs->recvbuf);
        sbufCFinish(cs->recvbuf);
        sbufRelease(&cs->recvbuf);
        cs->init = false;
    }

    memset(cs, 0, sizeof(ControlState));
}
