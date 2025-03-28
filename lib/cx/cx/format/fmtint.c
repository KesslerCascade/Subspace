#include "format_private.h"
#include "cx/utils/compare.h"
#include "cx/string/string_private.h"

enum IntOpts {
    FMT_IntMin      = 0x00010000,
    FMT_IntPrefix   = 0x00020000,
    FMT_IntUtfChar  = 0x00040000,
};

_Use_decl_annotations_
bool _fmtParseIntOpt(FMTVar *v, strref opt)
{
    uint32 val;
    if (strEq(opt, _S"prefix")) {
        v->flags |= FMT_IntPrefix;
        return true;
    } else if (strEq(opt, _S"utfchar")) {
        v->flags |= FMT_IntUtfChar;
        return true;
    } else if (strEq(opt, _S"hex")) {
        v->fmtdata[0] = 16;
        return true;
    } else if (strEq(opt, _S"octal")) {
        v->fmtdata[0] = 8;
        return true;
    } else if (strEq(opt, _S"binary")) {
        v->fmtdata[0] = 2;
        return true;
    } else if (strBeginsWith(opt, _S"base:")) {
        strSubStr(&v->tmp, opt, 5, strEnd);
        if (strToUInt32(&val, v->tmp, 10, true) && val >= 2 && val <= 36)
            v->fmtdata[0] = val;
        return true;
    } else if (strBeginsWith(opt, _S"min:")) {
        strSubStr(&v->tmp, opt, 4, strEnd);
        if (strToUInt32(&val, v->tmp, 10, true)) {
            v->flags |= FMT_IntMin;
            v->fmtdata[1] = val;
        }
        return true;
    }
    return false;
}

_Use_decl_annotations_
bool _fmtParseIntFinalize(FMTVar *v)
{
    // default to base 10
    if (v->fmtdata[0] == 0)
        v->fmtdata[0] = 10;

    if ((v->flags & FMT_IntPrefix) && !(v->fmtdata[0] == 8 || v->fmtdata[0] == 16))
        v->flags &= ~FMT_IntPrefix;

    // we handle upper/lower in the int formatter itself to exclude the prefix
    v->flags |= FMTVar_NoGenCase;

    // width will be padded in the int formatter
    v->flags |= FMTVar_NoGenWidth;

    return true;
}

_Use_decl_annotations_
bool _fmtInt(FMTVar *v, string *out)
{
    uint64 val = 0;
    bool neg = false;
    // explicitly use two's complement to make unsigned/positive and handle the edge case
    // see also strnum_int.c
    switch (stGetId(v->type)) {
    case stTypeId(int8):
        neg = *(int8*)v->data < 0;
        val = neg ? ((uint8)~*(uint8*)v->data + 1) : *(uint8*)v->data;
        break;
    case stTypeId(uint8):
        val = *(uint8*)v->data;
        break;
    case stTypeId(int16):
        neg = *(int16*)v->data < 0;
        val = neg ? ((uint16)~*(uint16*)v->data + 1) : *(uint16*)v->data;
        break;
    case stTypeId(uint16):
        val = *(uint16*)v->data;
        break;
    case stTypeId(int32):
        neg = *(int32*)v->data < 0;
        val = neg ? ((uint32)~*(uint32*)v->data + 1) : *(uint32*)v->data;
        break;
    case stTypeId(uint32):
        val = *(uint32*)v->data;
        break;
    case stTypeId(int64):
        neg = *(int64*)v->data < 0;
        val = neg ? ((uint64)~*(uint64*)v->data + 1) : *(uint64*)v->data;
        break;
    case stTypeId(uint64):
        val = *(uint64*)v->data;
        break;
    }

    if (v->flags & FMT_IntUtfChar) {
        // this doesn't actually print an integer, but does something different entirely
        uint8 buf[5];
        uint32 len = _strUTF8Encode(buf, (int32)val);
        if (len == 0)
            return false;
        buf[len] = 0;
        strCopy(out, (string)buf);
        return true;
    }

    uint8 buf[STRNUM_INTBUF];
    char sign = 0;
    char prefix[2] = { 0 };
    uint32 buflen, pfxlen = 0;
    uint8 *p;
    // we always pass positive unsigned values to strFromNum because extra leading zeros
    // may need to be added after the sign
    p = _strnum_u64toa(buf, &buflen, val, (uint16)v->fmtdata[0], (v->flags & FMT_IntMin) ? (uint32)v->fmtdata[1] : 0,
                   false, v->flags & FMTVar_Upper);

    if (neg)
        sign = '-';         // TODO: localization
    else if (v->flags & FMTVar_SignAlways)
        sign = '+';
    else if (v->flags & FMTVar_SignPrefix)
        sign = ' ';

    if (v->flags & FMT_IntPrefix) {
        if (v->fmtdata[0] == 8 && p[0] != '0') {
            prefix[0] = '0';
            pfxlen = 1;
        } else if (v->fmtdata[0] == 16) {
            prefix[0] = '0';
            prefix[1] = 'x';
            pfxlen = 2;
        }
    }

    strClear(out);
    uint32 totallen = buflen + pfxlen + (sign ? 1 : 0);
    uint32 width = clamplow(v->width, 0);
    uint32 diff = width - totallen;
    uint8 *obuf = strBuffer(out, max(totallen, width));
    uint8 *start = obuf;

    // integers never truncate to width, only expand
    if (totallen < width) {
        if (v->flags & FMTVar_LeadingZeros) {
            // do nothing here, fill in later
        } else if (v->flags & FMTVar_Left) {
            memset(obuf + totallen, ' ', diff);
        } else if (v->flags & FMTVar_Center) {
            size_t l1 = (width - totallen) / 2;
            memset(obuf, ' ', l1);
            start = obuf + l1;
            memset(start + totallen, ' ', diff - l1);
        } else {
            // FMTVar_Right
            memset(obuf, ' ', diff);
            start = obuf + diff;
        }
    }

    if (sign != 0)
        *start++ = sign;
    for (uint32 i = 0; i < pfxlen; i++)
        *start++ = prefix[i];

    if (totallen < width && (v->flags & FMTVar_LeadingZeros)) {
        // finally do this after the sign/prefix
        memset(start, '0', diff);
        start += diff;
    }

    memcpy(start, p, buflen);

    return true;
}
