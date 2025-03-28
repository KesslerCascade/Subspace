#include "string_private.h"
#include "cx/container/sarray.h"

_Use_decl_annotations_
bool strJoin(strhandle o, sa_string arr, strref sep)
{
    uint32 seplen = strLen(sep), seglen;
    int32 arrsize = saSize(arr);
    uint8 encoding = STR_ENCODING_MASK;
    int32 i;

    if (arrsize == 0) {
        _strReset(o, 0);
        return false;
    }
    if (!STR_CHECK_VALID(sep))
        sep = _strEmpty;

    uint32 sz = 0;
    for (i = 0; i < arrsize; i++) {
        sz += strLen(arr.a[i]) + (i > 0 ? seplen : 0);
        if (STR_CHECK_VALID(arr.a[i]))
            encoding &= _strHdr(arr.a[i]) & STR_ENCODING_MASK;
    }

    _strReset(o, sz);
    uint8 *p = _strBuffer(*o);

    if (STR_CHECK_VALID(arr.a[0])) {
        seglen = _strFastLen(arr.a[0]);
        _strFastCopy(arr.a[0], 0, p, seglen);
        p += seglen;
    }
    for (i = 1; i < arrsize; i++) {
        _strFastCopy(sep, 0, p, seplen);
        p += seplen;

        if (STR_CHECK_VALID(arr.a[i])) {
            seglen = _strFastLen(arr.a[i]);
            _strFastCopy(arr.a[i], 0, p, seglen);
            p += seglen;
        }
    }

    *p = 0;             // null terminator

    *_strHdrP(*o) &= ~STR_ENCODING_MASK;
    *_strHdrP(*o) |= encoding;
    _strSetLen(*o, sz);

    return true;
}
