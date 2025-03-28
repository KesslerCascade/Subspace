#include "string_private.h"
#include "cx/debug/error.h"

#ifdef _MSC_VER
// this function make use of intentional integer overflow
#pragma warning(disable:26450)
#endif

char _strnum_udigits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char _strnum_ldigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

// string to integer -------------------------------------------------------------------

#define STRNUM_IMPL(type, stype, utype, name, CUTOFF)        \
_Use_decl_annotations_                                          \
bool name(type *_Nonnull out, strref s, int base, bool strict)        \
{                                                            \
    utype acc;                                               \
    uint8 c;                                                 \
    utype cutoff;                                            \
    bool neg, any;                                           \
    int cutlim;                                              \
    uint32 i = 0;                                            \
                                                             \
    if(!s || strEmpty(s))                                          \
        return false;                                        \
                                                             \
    do {                                                     \
        c = _strFastChar(s, i++);                            \
    } while (isspace(c));                                    \
    if (c == '-') {                                          \
        neg = true;                                          \
        c = _strFastChar(s, i++);                            \
    } else {                                                 \
        neg = false;                                         \
        if (c == '+')                                        \
            c = _strFastChar(s, i++);                        \
    }                                                        \
                                                             \
    uint8 s1 = strGetChar(s, i);                             \
    uint8 s2 = strGetChar(s, i + 1);                         \
    if ((base == 0 || base == 16) &&                         \
        c == '0' && (s1 == 'x' || s1 == 'X') &&              \
        ((s2 >= '0' && s2 <= '9') ||                         \
        (s2 >= 'A' && s2 <= 'F') ||                          \
         (s2 >= 'a' && s2 <= 'f'))) {                        \
        i++;                                                 \
        c = _strFastChar(s, i++);                            \
        base = 16;                                           \
    }                                                        \
    if (base == 0)                                           \
        base = 10;                                           \
    acc = 0;                                                 \
    any = false;                                             \
    if (base < 2 || base > 36) {                             \
        cxerr = CX_InvalidArgument;                          \
        return false;                                        \
    }                                                        \
                                                             \
    cutoff = CUTOFF;                                         \
    cutlim = cutoff % base;                                  \
    cutoff /= base;                                          \
    for (;; c = _strFastChar(s, i++)) {                      \
        if (c >= '0' && c <= '9')                            \
            c -= '0';                                        \
        else if (c >= 'A' && c <= 'Z')                       \
            c -= 'A' - 10;                                   \
        else if (c >= 'a' && c <= 'z')                       \
            c -= 'a' - 10;                                   \
        else break;                                          \
        if (c >= base)                                       \
            break;                                           \
        if (acc > cutoff || (acc == cutoff && c > cutlim)) { \
            cxerr = CX_Range;                                \
            return false;                                    \
        }                                                    \
        any = true;                                          \
        acc *= base;                                         \
        acc += c;                                            \
    }                                                        \
                                                             \
    if (!any) {                                              \
        cxerr = CX_InvalidArgument;                          \
        return false;                                        \
    }                                                        \
                                                             \
    if (strict && i < strLen(s) + 1) {                       \
        cxerr = CX_InvalidArgument;                          \
        return false;                                        \
    }                                                        \
                                                             \
    *out = !neg ? acc : -(stype)acc;                         \
    return true;                                             \
}

STRNUM_IMPL(int32, int32, uint32, strToInt32, neg ? (uint32)-(MIN_INT32 + MAX_INT32) + MAX_INT32 : MAX_INT32)
STRNUM_IMPL(uint32, int32, uint32, strToUInt32, MAX_UINT32)
STRNUM_IMPL(int64, int64, uint64, strToInt64, neg ? (uint64)-(MIN_INT64 + MAX_INT64) + MAX_INT64 : MAX_INT64)
STRNUM_IMPL(uint64, int64, uint64, strToUInt64, MAX_UINT64)

// integer to string -------------------------------------------------------------------

_Use_decl_annotations_
uint8 *_Nonnull _strnum_u64toa(uint8 buf[STRNUM_INTBUF], uint32 *_Nullable len, uint64 val, uint16 base, uint32 mindigits, char sign, bool upper)
{
    uint8 *p = buf + STRNUM_INTBUF;
    char *cset;
    uint32 val32;

    base = base ? base : 10;
    cset = upper ? _strnum_udigits : _strnum_ldigits;

    *--p = 0;       // null terminator

    while (val > 0xffffffff) {
        *--p = cset[val % base];
        val /= base;
    }
    val32 = (uint32)val;
    do {
        *--p = cset[val32 % base];
        val32 /= base;
    } while (val32 > 0);

    uint32 buflen = (uint32)(buf + STRNUM_INTBUF - p - 1);

    // mindigits applies *before* sign is added
    mindigits = clamphigh(mindigits, STRNUM_INTBUF - 2);
    if (buflen < mindigits) {
        for (size_t i = 0; i < mindigits - buflen; i++)
            *--p = '0';
        buflen = mindigits;
    }

    if (sign != 0) {
        *--p = sign;
        ++buflen;
    }

    if (len)
        *len = buflen;
    return p;
}

static _meta_inline bool _strFromIntHelper(strhandle out, uint64 val, uint16 base, uint32 mindigits, char sign, bool upper)
{
    uint8 buf[STRNUM_INTBUF];
    uint32 buflen;

    uint8 *p = _strnum_u64toa(buf, &buflen, val, base, mindigits, sign, upper);

    strClear(out);

    if (buflen == 0)
        return false;

    uint8 *obuf = strBuffer(out, buflen);
    memcpy(obuf, p, buflen);

    return true;
}

// TODO: localize the sign character
_Use_decl_annotations_
bool strFromInt32(strhandle out, int32 i, uint16 base)
{
    if (i >= 0)
        return _strFromIntHelper(out, (uint64)i, base, 0, 0, false);
    // this looks strange but it properly handles the 'weird' minimum signed int
    uint32 val = (uint32)(~i) + 1;
    return _strFromIntHelper(out, val, base, 0, '-', false);
}

_Use_decl_annotations_
bool strFromUInt32(strhandle out, uint32 i, uint16 base)
{
    return _strFromIntHelper(out, i, base, 0, 0, false);
}

_Use_decl_annotations_
bool strFromInt64(strhandle out, int64 i, uint16 base)
{
    if (i >= 0)
        return _strFromIntHelper(out, (uint64)i, base, 0, 0, false);
    return _strFromIntHelper(out, (uint64)(~i) + 1, base, 0, '-', false);
}

_Use_decl_annotations_
bool strFromUInt64(strhandle out, uint64 i, uint16 base)
{
    return _strFromIntHelper(out, i, base, 0, 0, false);
}
