#include "string_private.h"

_Use_decl_annotations_
_Pure bool strEq(strref s1, strref s2)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    if (_strFastLen(s1) != _strFastLen(s2))
        return false;               // early out if lengths do not match

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);

    // strings are the same size, so both are guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(i1.len, i2.len);
        if (clen == 0)
            return true;            // end of strings, everything matched
        if (memcmp(i1.bytes, i2.bytes, clen))
            return false;           // mismatch
        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
    }

    return false;       // unreachable
}

_Use_decl_annotations_
_Pure int32 strCmp(strref s1, strref s2)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    int ret;

    // these are NOT guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(i1.len, i2.len);
        ret = memcmp(i1.bytes, i2.bytes, clen);
        if (ret != 0)
            return ret;             // mismatch

        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);

        if (i1.len == 0 && i2.len == 0)
            return 0;               // both hit end, match
        if (i1.len == 0)
            return -1;
        else if (i2.len == 0)
            return 1;
    }

    return 0;       // unreachable
}

_Use_decl_annotations_
_Pure bool strEqi(strref s1, strref s2)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    if (_strFastLen(s1) != _strFastLen(s2))
        return false;               // early out if lengths do not match

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);

    // strings are the same size, so both are guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(i1.len, i2.len);
        if (clen == 0)
            return true;                // end of strings, everything matched

        for (uint32 j = 0; j < clen; j++) {
            if (tolower(i1.bytes[j]) != tolower(i2.bytes[j]))
                return false;
        }
        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
    }

    return false;           // unreachacle
}

_Use_decl_annotations_
_Pure int32 strCmpi(strref s1, strref s2)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    int ret = 0;

    // these are NOT guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(i1.len, i2.len);
        for (uint32 j = 0; j < clen; j++) {
            ret = tolower(i1.bytes[j]) - tolower(i2.bytes[j]);
            if (ret != 0)
                return ret;
        }

        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);

        if (i1.len == 0 && i2.len == 0)
            return 0;               // both hit end, match
        if (i1.len == 0)
            return -1;
        else if (i2.len == 0)
            return 1;
    }

    return 0;           // unreachable
}

_Use_decl_annotations_
_Pure bool strRangeEq(strref s1, strref s2, int32 off, uint32 len)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    // negative offset means relative to end of string
    if (off < 0)
        off += _strFastLen(s1);
    if (off < 0)
        return false;

    if (clamphigh(_strFastLen(s1) - off, len) != clamphigh(_strFastLen(s2), len))
        return false;               // early out if lengths do not match

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    striSeek(&i1, off, STRI_BYTE, STRI_SET);

    // strings are the same size, so both are guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(min(i1.len, i2.len), len);
        if (clen == 0)
            return true;            // end of strings, everything matched
        if (memcmp(i1.bytes, i2.bytes, clen))
            return false;           // mismatch
        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
        len -= clen;
    }

    return false;       // unreachable
}

_Use_decl_annotations_
_Pure int32 strRangeCmp(strref s1, strref s2, int32 off, uint32 len)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    // negative offset means relative to end of string
    if (off < 0)
        off += _strFastLen(s1);

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    striSeek(&i1, off, STRI_BYTE, STRI_SET);
    int ret;

    // these are NOT guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(min(i1.len, i2.len), len);
        ret = memcmp(i1.bytes, i2.bytes, clen);
        if (ret != 0)
            return ret;             // mismatch

        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
        len -= clen;

        if (len == 0)
            return 0;               // both hit end, match
        if (i1.len == 0)
            return -1;
        else if (i2.len == 0)
            return 1;
    }

    return 0;       // unreachable
}

_Use_decl_annotations_
_Pure bool strRangeEqi(strref s1, strref s2, int32 off, uint32 len)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    // negative offset means relative to end of string
    if (off < 0)
        off += _strFastLen(s1);

    if (clamphigh(_strFastLen(s1) - off, len) != clamphigh(_strFastLen(s2), len))
        return false;               // early out if lengths do not match

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    striSeek(&i1, off, STRI_BYTE, STRI_SET);

    // strings are the same size, so both are guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(min(i1.len, i2.len), len);
        if (clen == 0)
            return true;                // end of strings, everything matched

        for (uint32 j = 0; j < clen; j++) {
            if (tolower(i1.bytes[j]) != tolower(i2.bytes[j]))
                return false;
        }
        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
        len -= clen;
    }

    return false;           // unreachacle
}

_Use_decl_annotations_
_Pure int32 strRangeCmpi(strref s1, strref s2, int32 off, uint32 len)
{
    if (!STR_CHECK_VALID(s1))
        s1 = _strEmpty;
    if (!STR_CHECK_VALID(s2))
        s2 = _strEmpty;

    // negative offset means relative to end of string
    if (off < 0)
        off += _strFastLen(s1);

    striter i1, i2;
    striBorrow(&i1, s1);
    striBorrow(&i2, s2);
    striSeek(&i1, off, STRI_BYTE, STRI_SET);
    int ret = 0;

    // these are NOT guaranteed to hit the end at the same time
    for (;;) {
        uint32 clen = min(min(i1.len, i2.len), len);
        for (uint32 j = 0; j < clen; j++) {
            ret = tolower(i1.bytes[j]) - tolower(i2.bytes[j]);
            if (ret != 0)
                return ret;
        }

        striSeek(&i1, clen, STRI_BYTE, STRI_CUR);
        striSeek(&i2, clen, STRI_BYTE, STRI_CUR);
        len -= clen;

        if (len == 0)
            return 0;               // both hit end, match
        if (i1.len == 0)
            return -1;
        else if (i2.len == 0)
            return 1;
    }

    return 0;           // unreachable
}

_Use_decl_annotations_
_Pure bool strBeginsWith(strref s1, strref s2)
{
    return strRangeEq(s1, s2, 0, strLen(s2));
}

_Use_decl_annotations_
_Pure bool strBeginsWithi(strref s1, strref s2)
{
    return strRangeEqi(s1, s2, 0, strLen(s2));
}

_Use_decl_annotations_
_Pure bool strEndsWith(strref s1, strref s2)
{
    return strRangeEq(s1, s2, -(int32)strLen(s2), strLen(s2));
}

_Use_decl_annotations_
_Pure bool strEndsWithi(strref s1, strref s2)
{
    return strRangeEqi(s1, s2, -(int32)strLen(s2), strLen(s2));
}
