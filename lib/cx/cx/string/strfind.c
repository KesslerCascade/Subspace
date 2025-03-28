#include "string_private.h"

_Use_decl_annotations_
int32 _strFindChar(strref_v s, int32 b, char find)
{
    uint32 slen, i;

    slen = _strFastLen(s);
    // allow negative starting index to mean from the end of the string
    if (b < 0)
        i = max(0, slen + b);
    else
        i = min((uint32)b, slen);

    striter it;
    striBorrow(&it, s);
    striSeek(&it, i, STRI_BYTE, STRI_SET);
    while (it.len > 0) {
        for (i = 0; i < it.len; i++) {
            if (it.bytes[i] == find)
                return (int32)(it.off + i);
        }
        striNext(&it);
    }

    return -1;
}

_Use_decl_annotations_
int32 _strFindCharR(strref_v s, int32 e, char find)
{
    // Conventional wisdom was wrong. Actually scanning backwards turns out to be
    // about 20% faster on average, probably because the conditions to check are
    // much simpler.

    uint32 slen;
    int32 i;

    slen = _strFastLen(s);
    // negative e indexes from the end of the string
    if (e < 0)
        slen = ((uint32)(-e) < slen) ? slen + e : 0;
    else if (e != strEnd)       // e == strEnd means the end of the string
        slen = min((uint32)e, slen);

    striter it;
    striBorrowRev(&it, s);
    while (it.len > 0) {
        if (it.off < slen) {
            for (i = min(it.len, slen - it.off) - 1; i >= 0; --i) {
                if (it.bytes[i] == find)
                    return i + it.off;
            }
        }
        striPrev(&it);
    }

    return -1;
}

// comparison helper that can handle degenerate case where
// string and substring are both ropes and don't have segments
// that line up cleanly
static inline bool striterEq(_In_ striter *_Nonnull istr_in, _In_ striter *_Nonnull isub_in)
{
    // borrow iterator state
    striter istr = *istr_in;
    striter isub = *isub_in;
    for (;;) {
        uint32 clen = min(istr.len, isub.len);
        if (clen == 0)
            return !isub.len;       // if end of isub, everything matched

        if (memcmp(istr.bytes, isub.bytes, clen))
            return false;           // mismatch

        striSeek(&istr, clen, STRI_BYTE, STRI_CUR);
        striSeek(&isub, clen, STRI_BYTE, STRI_CUR);
    }

    return false;
}

_Use_decl_annotations_
int32 strFind(strref s, int32 b, strref find)
{
    uint32 off, slen, i;

    if (!STR_CHECK_VALID(s) || strEmpty(find))
        return -1;

    if (_strFastLen((strref_v)find) == 1 && !(_strHdr(find) & STR_ROPE)) {
        // optimization for simple case
        return _strFindChar(s, b, _strBuffer(find)[0]);
    }

    slen = _strFastLen(s);
    // allow negative starting index to mean from the end of the string
    if (b < 0)
        off = max(0, slen + b);
    else
        off = min((uint32)b, slen);

    if (slen < off + _strFastLen(find))
        return -1;          // nonsensical, can't possibly fit

    // faster to search for first character of find string in a tight loop
    char fchr = strGetChar(find, 0);

    striter istr, isub;
    striBorrow(&istr, s);
    striBorrow(&isub, find);
    striSeek(&istr, off, STRI_BYTE, STRI_SET);
    while (istr.len > 0) {
        for (i = 0; i < istr.len; i++) {
            if (istr.bytes[i] == fchr) {
                striSeek(&istr, istr.off + i, STRI_BYTE, STRI_SET);
                if (striterEq(&istr, &isub))
                    return (int32)istr.off;
                i = 0;                  // we reset the iterator, start at beginning
            }
        }
        striNext(&istr);
    }

    return -1;
}

_Use_decl_annotations_
int32 strFindR(strref s, int32 e, strref find)
{
    // see _strFindCharR and strFind for implementation notes
    uint32 slen;
    int32 i;
    if (!STR_CHECK_VALID(s) || strEmpty(find))
        return -1;

    if (_strFastLen((strref_v)find) == 1 && !(_strHdr(find) & STR_ROPE)) {
        // optimization for simple case
        return _strFindCharR(s, e, _strBuffer(find)[0]);
    }

    slen = _strFastLen(s);
    // negative e indexes from the end of the string
    if (e < 0)
        slen = ((uint32)(-e) < slen) ? slen + e : 0;
    else if (e != strEnd)       // e == strEnd means the end of the string
        slen = min((uint32)e, slen);

    // faster to search for first character of find string in a tight loop
    char fchr = strGetChar(find, 0);

    striter istr, isub;
    striBorrowRev(&istr, s);
    striBorrow(&isub, find);
    while (istr.len > 0) {
        // complex condition is to handle "starting" at slen
        if (istr.off < slen) {
            for (i = min(istr.len, slen - istr.off) - 1; i >= 0; --i) {
                if (istr.bytes[i] == fchr) {
                    striter temp = istr;
                    striSeek(&temp, istr.off + i, STRI_BYTE, STRI_SET);
                    if (striterEq(&temp, &isub))
                        return (int32)temp.off;
                }
            }
        }
        striPrev(&istr);
    }

    return -1;
}
