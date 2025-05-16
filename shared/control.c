#ifndef SUBSPACE_GAME
#define XALLOC_REMAP_MALLOC
#endif

#include "control.h"
#include "netsocket.h"

#ifdef SUBSPACE_GAME
#include "ftl/stdlib.h"
#include "minicrt.h"
#define control_malloc smalloc
#define control_free   sfree
#else
#include <cx/xalloc.h>
#define control_malloc xa_malloc
#define control_free   xa_free
#endif

#define TCPBUF_SEND 65536
#define TCPBUF_RECV 65536

static const uint8_t syncmagic[4] = { 0x0e, 0x19, 0x01, 0x17 };

static void recvNotify(StreamBuffer* sb, size_t sz, void* ctx)
{
    // TODO: Pass on to control receive callback when a complete message is received
}

bool sendToSocket(StreamBuffer* sb, const uint8_t* buf, size_t off, size_t sz, void* ctx)
{
    ControlState* cs = (ControlState*)ctx;
    intptr_t bsent   = send(cs->sock, buf + off, sz, 0);
    cs->lastsent += bsent > 0 ? bsent : 0;
    return false;   // always peek, because send() might short write and we don't want to consume
                    // all the data
}

static void sendNotify(StreamBuffer* sb, size_t sz, void* ctx)
{
    ControlState* cs = (ControlState*)ctx;
    while (sz > 0) {
        size_t tosend = min(sz, TCPBUF_SEND / 2);
        bool shortsnd = true;
        cs->lastsent  = 0;
        if (sbufCSend(sb, sendToSocket, tosend) && cs->lastsent > 0) {
            sbufCSkip(sb, cs->lastsent);
            sz -= cs->lastsent;

            if (cs->lastsent == tosend)
                shortsnd = false;
            cs->lastsent = 0;
        }

        if (shortsnd)
            break;   // couldn't send all of it, buffer might be full
    }
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
        // just some temporary space, mostly for the game portion so it doesn't have to call smalloc
        // a lot
        cs->tmprecv = control_malloc(TCPBUF_RECV);

        cs->recvbuf = sbufCreate(TCPBUF_RECV * 4);
        cs->sendbuf = sbufCreate(TCPBUF_SEND * 4);

        sbufPRegisterPush(cs->recvbuf, NULL, cs);
        sbufCRegisterPush(cs->recvbuf, recvNotify, NULL, cs);

        sbufPRegisterPush(cs->sendbuf, NULL, cs);
        sbufCRegisterPush(cs->sendbuf, sendNotify, NULL, cs);
    }
}

bool controlSend(ControlState* cs)
{
    bool ret         = false;
    StreamBuffer* sb = cs->sendbuf;
    while (sbufCAvail(sb) > 0) {
        size_t tosend = min(sbufCAvail(sb), TCPBUF_SEND / 2);
        bool shortsnd = true;
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
    case CF_FLOAT64:
        sbufPWrite(sb, v, 8);
        ret = 8;
        break;
    case CF_BOOL:
        sbufPWrite(sb, v, 1);
        ret = 1;
        break;
    case CF_STRING: {
#ifdef SUBSPACE_GAME
        uint32_t len = strlen(*(char**)v);
#else
        uint32_t len = strLen(*(strref*)v);
#endif
        uint16_t ssz = min(len, 65536);   // TODO: Some kind of safety check here
        sbufPWrite(sb, (uint8_t*)&ssz, 2);
#ifdef SUBSPACE_GAME
        sbufPWrite(sb, *(char**)v, ssz);
#else
        sbufPWriteStr(sb, *(strref*)v);
#endif
        ret = len + 2;
        break;
    }
    case CF_RAW:
        sbufPWrite(sb, v, rawsz);
        ret = rawsz;
        break;
    }

    return ret;
}

// add a message to the output buffer and try to send it
bool controlPutMsg(ControlState* cs, ControlMsgHeader* hdr, ControlField** fields)
{
    memcpy(hdr->sync, syncmagic, 4);
    hdr->size  = sizeof(ControlMsgHeader);
    hdr->msgid = cs->nextid++;

    // go through the fields and add up their size
    for (uint32_t i = 0; i < hdr->nfields; i++) {
        ControlFieldHeader* cfh = &fields[i]->h;
        size_t dsize            = 0;

        if (cfh->ftype == CF_STRING && (cfh->flags & CF_ARRAY)) {
            // special case, have to count all the strings, plus lengths
            dsize = 4;
            for (uint32_t j = 0; j < fields[i]->count; j++) {
#ifdef SUBSPACE_GAME
                dsize += strlen(fields[i]->d.cfd_str_arr[j]) + 2;
#else
                dsize += strLen(fields[i]->d.cfd_str_arr.a[j]) + 2;
#endif
            }
        } else {
            switch (cfh->ftype) {
            case CF_INT:
            case CF_FLOAT32:
                dsize = 4;
                break;
            case CF_FLOAT64:
                dsize = 8;
                break;
            case CF_BOOL:
                dsize = 1;
                break;
            case CF_STRING:
#ifdef SUBSPACE_GAME
                dsize = strlen(fields[i]->d.cfd_str) + 2;
#else
                dsize = strLen(fields[i]->d.cfd_str) + 2;
#endif
                break;
            case CF_RAW:
                dsize = fields[i]->count;
            }

            if (cfh->flags & CF_ARRAY) {
                dsize *= fields[i]->count;
                dsize += 4;
            }
        }

        cfh->size = sizeof(ControlFieldHeader) + dsize;
        // align to 4-byte boundary
        cfh->size = ((cfh->size + 3) / 4) * 4;
        hdr->size += cfh->size;
    }

    // now write out the message
    StreamBuffer* sb = cs->sendbuf;
    sbufPWrite(sb, (uint8_t*)hdr, sizeof(ControlMsgHeader));

    for (uint32_t i = 0; i < hdr->nfields; i++) {
        ControlFieldHeader* cfh = &fields[i]->h;
        int32_t pad             = cfh->size - sizeof(ControlFieldHeader);

        sbufPWrite(sb, (uint8_t*)cfh, sizeof(ControlFieldHeader));
        if (cfh->flags & CF_ARRAY) {
            sbufPWrite(sb, (uint8_t*)&fields[i]->count, 4);
            pad -= 4;
            for (uint32_t j = 0; j < fields[i]->count; j++) {
                switch (cfh->ftype) {
                case CF_INT:
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_int_arr[j], 0);
                    break;
                case CF_FLOAT32:
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_float32_arr[j], 0);
                    break;
                case CF_FLOAT64:
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_float64_arr[j], 0);
                    break;
                case CF_BOOL:
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_bool_arr[j], 0);
                    break;
                case CF_STRING:
#ifdef SUBSPACE_GAME
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_str_arr[j], 0);
#else
                    pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d.cfd_str_arr.a[j], 0);
#endif
                    break;
                }
            }
        } else {
            pad -= writeOneVal(sb, cfh->ftype, &fields[i]->d, fields[i]->count);
        }

        for (int k = 0; k < pad; k++) {
            uint8_t zero[1] = { 0 };
            sbufPWrite(sb, zero, 1);
        }
    }

    // try to send the outbound data
    return controlSend(cs);
}

// decode message parts from a FULLY received message in the buffer (see controlMsgReady)
bool controlGetHeader(ControlState* cs, ControlMsgHeader* hdr)
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

static void* allocBytes(size_t sz, void* orig, int allocmode, uint32_t count)
{
    switch (allocmode) {
    case CF_ALLOC_AUTO:
        return malloc(sz);
#ifdef SUBSPACE_GAME
    case CF_ALLOC_SALLOC:
        return smalloc(sz);
#endif
    case CF_ALLOC_PRE:
        if (sz <= count)
            return *((void**)orig);
        break;
    }

    return NULL;
}

static void freeBytes(void* ptr, int allocmode)
{
    switch (allocmode) {
    case CF_ALLOC_AUTO:
        free(ptr);
        break;
#ifdef SUBSPACE_GAME
    case CF_ALLOC_SALLOC:
        sfree(ptr);
        break;
#endif
    }
}

static bool parseField(StreamBuffer* sb, int ftype, void* dest, int allocmode, size_t* pad,
                       uint32_t count)
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

#ifdef SUBSPACE_GAME
        char* ptr = allocBytes(len + 1, dest, allocmode, count);
        if (!ptr)
            return false;

        sbufCRead(sb, ptr, len, &didread);
        ptr[len]      = '\0';
        *(char**)dest = ptr;
#else
        string nstr  = 0;
        uint8_t* buf = strBuffer(&nstr, len);
        sbufCRead(sb, buf, len, &didread);
        *(string*)dest = nstr;
#endif
        *pad -= len;
        return true;
    }
    case CF_RAW:
        // read everything that's left in the field
        uint8_t* ptr = allocBytes(*pad, dest, allocmode, count);
        if (!ptr)
            return false;
        sbufCRead(sb, ptr, *pad, &didread);
        *pad = 0;
        return true;
    }
    return false;
}

bool controlGetField(ControlState* cs, ControlField* field, int allocmode)
{
    StreamBuffer* sb = cs->recvbuf;
    size_t didread;
    sbufCRead(sb, (uint8_t*)&field->h, sizeof(ControlFieldHeader), &didread);

    ControlFieldHeader* hdr = &field->h;
    size_t pad              = hdr->size - sizeof(ControlFieldHeader);
    if (allocmode != CF_ALLOC_PRE)
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

        switch (hdr->ftype) {
        case CF_INT:
            field->d.cfd_int_arr = (int*)allocBytes(field->count * sizeof(int), NULL, allocmode, 0);
            if (!field->d.cfd_int_arr)
                return false;
            break;
        case CF_FLOAT32:
            field->d.cfd_float32_arr = (float*)
                allocBytes(field->count * sizeof(float), NULL, allocmode, 0);
            if (!field->d.cfd_int_arr)
                return false;
            break;
        case CF_FLOAT64:
            field->d.cfd_float64_arr = (double*)
                allocBytes(field->count * sizeof(double), NULL, allocmode, 0);
            if (!field->d.cfd_int_arr)
                return false;
            break;
        case CF_BOOL:
            field->d.cfd_bool_arr = (bool*)
                allocBytes(field->count * sizeof(bool), NULL, allocmode, 0);
            if (!field->d.cfd_bool_arr)
                return false;
            break;
        case CF_STRING:
#ifdef SUBSPACE_GAME
            field->d.cfd_str_arr = (char**)
                allocBytes(field->count * sizeof(char*), NULL, allocmode, 0);
            if (!field->d.cfd_str_arr)
                return false;
#else
            saInit(&field->d.cfd_str_arr, string, field->count);
            saSetSize(&field->d.cfd_str_arr, field->count);
#endif
            break;
        }

        for (uint32_t j = 0; j < field->count; j++) {
            bool success = false;
            switch (hdr->ftype) {
            case CF_INT:
                success = parseField(sb, hdr->ftype, &field->d.cfd_int_arr[j], allocmode, &pad, 0);
                break;
            case CF_FLOAT32:
                success = parseField(sb,
                                     hdr->ftype,
                                     &field->d.cfd_float32_arr[j],
                                     allocmode,
                                     &pad,
                                     0);
                break;
            case CF_FLOAT64:
                success = parseField(sb,
                                     hdr->ftype,
                                     &field->d.cfd_float64_arr[j],
                                     allocmode,
                                     &pad,
                                     0);
                break;
            case CF_BOOL:
                success = parseField(sb, hdr->ftype, &field->d.cfd_bool_arr[j], allocmode, &pad, 0);
                break;
            case CF_STRING:
#ifdef SUBSPACE_GAME
                success = parseField(sb, hdr->ftype, &field->d.cfd_str_arr[j], allocmode, &pad, 0);
#else
                success = parseField(sb,
                                     hdr->ftype,
                                     &field->d.cfd_str_arr.a[j],
                                     allocmode,
                                     &pad,
                                     0);
#endif
                break;
            }
            if (!success)
                return false;
        }
    } else {
        if (!parseField(sb, hdr->ftype, &field->d, allocmode, &pad, field->count))
            return false;
    }

    // if there's any padding bytes left, skip them
    if (pad > 0)
        sbufCSkip(sb, pad);

    cs->left -= hdr->size;

    return true;
}

ControlMsg* controlGetMsg(ControlState* cs, int allocmode)
{
    if (allocmode == CF_ALLOC_NEVER || allocmode == CF_ALLOC_PRE)
        return NULL;

    ControlMsg* ret = allocBytes(sizeof(ControlMsg), NULL, allocmode, 0);
    if (!ret)
        goto out;

    if (!controlGetHeader(cs, &ret->hdr) || ret->hdr.nfields > MAX_CONTROL_ARRAY) {
        freeBytes(ret, allocmode);
        ret = NULL;
        goto out;
    }

    if (ret->hdr.nfields > 0) {
        ret->fields = allocBytes(ret->hdr.nfields * sizeof(void*), NULL, allocmode, 0);
        if (!ret->fields) {
            freeBytes(ret, allocmode);
            ret = NULL;
            goto out;
        }

        for (uint32_t i = 0; i < ret->hdr.nfields; i++) {
            ret->fields[i] = allocBytes(sizeof(ControlField), NULL, allocmode, 0);
            if (!ret->fields[i] || !controlGetField(cs, ret->fields[i], allocmode)) {
                for (uint32_t j = 0; j < i; j++) {
                    freeBytes(ret->fields[j], allocmode);
                }
                freeBytes(ret->fields, allocmode);
                freeBytes(ret, allocmode);
                ret = NULL;
                goto out;
            }
        }
    }

out:
    controlRecvDone(cs);
    return ret;
}

void controlRecvDone(ControlState* cs)
{
    StreamBuffer* sb = cs->recvbuf;
    sbufCSkip(sb, cs->left);
    cs->recvhdr = false;
    cs->left    = 0;
    cs->nfields = 0;
    cs->nfield  = 0;
}

void controlFieldFree(ControlField* field, int allocmode)
{
    if (field->h.flags & CF_ARRAY) {
        switch (field->h.ftype) {
        case CF_INT:
            freeBytes(field->d.cfd_int_arr, allocmode);
            break;
        case CF_FLOAT32:
            freeBytes(field->d.cfd_float32_arr, allocmode);
            break;
        case CF_FLOAT64:
            freeBytes(field->d.cfd_float64_arr, allocmode);
            break;
        case CF_BOOL:
            freeBytes(field->d.cfd_bool_arr, allocmode);
            break;
        case CF_STRING:
#ifdef SUBSPACE_GAME
            for (uint32_t i = 0; i < field->count; i++) {
                freeBytes(field->d.cfd_str_arr[i], allocmode);
            }
            freeBytes(field->d.cfd_str_arr, allocmode);
#else
            saDestroy(&field->d.cfd_str_arr);
#endif
            break;
        }
    } else {
        switch (field->h.ftype) {
        case CF_STRING:
#ifdef SUBSPACE_GAME
            freeBytes(field->d.cfd_str, allocmode);
#else
            strDestroy(&field->d.cfd_str);
#endif
            break;
        case CF_RAW:
            freeBytes(field->d.cfd_raw, allocmode);
            break;
        }
    }
}

void controlFieldFreeMulti(uint32_t nfields, ControlField** fields, int allocmode, bool freearr)
{
    for (uint32_t i = 0; i < nfields; i++) {
        controlFieldFree(fields[i], allocmode);
        freeBytes(fields[i], allocmode);
    }
    if (freearr && nfields > 0)
        freeBytes(fields, allocmode);
}

void controlMsgFree(ControlMsg* msg, int allocmode)
{
    controlFieldFreeMulti(msg->hdr.nfields, msg->fields, allocmode, true);
    freeBytes(msg, allocmode);
}

ControlMsg* controlAllocMsg(int nfields, int allocmode)
{
    if (allocmode == CF_ALLOC_NEVER || allocmode == CF_ALLOC_PRE)
        return NULL;

    ControlMsg* ret = allocBytes(sizeof(ControlMsg), NULL, allocmode, 0);
    if (!ret)
        return NULL;

    memset(&ret->hdr, 0, sizeof(ControlMsgHeader));
    ret->hdr.nfields = nfields;
    if (nfields > 0) {
        ret->fields = allocBytes(sizeof(void*) * nfields, NULL, allocmode, 0);

        for (int i = 0; i < nfields; i++) {
            ret->fields[i] = allocBytes(sizeof(ControlField), NULL, allocmode, 0);
            memset(ret->fields[i], 0, sizeof(ControlField));
        }
    }
    return ret;
}

ControlMsg* controlNewMsg(const char* cmd, int nfields)
{
    ControlMsg* ret;

    ret = controlAllocMsg(nfields, CF_ALLOC_AUTO);
    strncpy(ret->hdr.cmd, cmd, sizeof(ret->hdr.cmd) - 1);
    return ret;
}

void controlMsgInt(ControlMsg* msg, int nfield, const char* name, int val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype   = CF_INT;
    f->d.cfd_int = val;
}

void controlMsgUInt(ControlMsg* msg, int nfield, const char* name, unsigned int val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype = CF_INT;
    f->h.flags |= CF_UNSIGNED;
    f->d.cfd_uint = val;
}

void controlMsgFloat32(ControlMsg* msg, int nfield, const char* name, float val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype       = CF_FLOAT32;
    f->d.cfd_float32 = val;
}

void controlMsgFloat64(ControlMsg* msg, int nfield, const char* name, double val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype       = CF_FLOAT64;
    f->d.cfd_float64 = val;
}

void controlMsgBool(ControlMsg* msg, int nfield, const char* name, bool val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype    = CF_BOOL;
    f->d.cfd_bool = val;
}

#ifdef SUBSPACE_GAME
void controlMsgStr(ControlMsg* msg, int nfield, const char* name, const char* val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype = CF_STRING;

    size_t sz    = strlen(val) + 1;
    f->d.cfd_str = allocBytes(sz, NULL, CF_ALLOC_AUTO, 0);
    if (!f->d.cfd_str)
        return;
    memcpy(f->d.cfd_str, val, sz);
}
#else
void controlMsgStr(ControlMsg* msg, int nfield, const char* name, strref val)
{
    if (nfield > msg->hdr.nfields)
        return;
    ControlField* f = msg->fields[nfield];
    strncpy(f->h.name, name, sizeof(f->h.name) - 1);
    f->h.ftype = CF_STRING;
    strDup(&f->d.cfd_str, val);
}
#endif

ControlField* controlMsgFindField(ControlMsg* msg, const char* name)
{
    for (int i = 0; i < msg->hdr.nfields; i++) {
        if (!strcmp(msg->fields[i]->h.name, name))
            return msg->fields[i];
    }
    return NULL;
}

void controlStateDestroy(ControlState* cs)
{
    // we assume that the socket is closed and abandoned
    if (cs->init) {
        control_free(cs->tmprecv);

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
