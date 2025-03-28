#include "string_private.h"
#include "cx/utils/scratch.h"

_Use_decl_annotations_
bool strValidUTF8(strref s)
{
    if (!STR_CHECK_VALID(s))
        return false;

    // check if it's cached
    if (_strHdr(s) & STR_UTF8)
        return true;

    striter it;
    uint32 len = _strFastLen(s);
    striBorrow(&it, s);

    while (len > 0) {
        uint32 seqlen = _strUTF8Decode(&it, NULL);
        if (seqlen == 0)                // decode failure
            return false;
        devAssert(seqlen <= len);       // should be impossible to have decoded more than we have
        len -= seqlen;
    }

    // we shouldn't have anything left over
    devAssert(len == 0);

    // if we allocated this string, mark it as UTF-8
    if (_strHdr(s) & STR_ALLOC)
        *_strHdrP(s) |= STR_UTF8;

    return true;
}

_When_(s == NULL, _Post_equal_to_(false))
bool strValidASCII(_In_opt_ strref s)
{
    if (!STR_CHECK_VALID(s))
        return false;

    // check if it's cached
    if (_strHdr(s) & STR_ASCII)
        return true;

    striter it;
    for (striBorrow(&it, s); it.len > 0; striNext(&it)) {
        for (uint32 i = 0; i < it.len; i++) {
            if (it.bytes[i] & 0x80)
                return false;       // non-ASCII character detected
        }
    }

    // if we allocated this string, mark it as ASCII
    if (_strHdr(s) & STR_ALLOC)
        *_strHdrP(s) |= STR_ASCII | STR_UTF8;           // ASCII is UTF-8 by default

    return true;
}

_Use_decl_annotations_
size_t strToUTF16(strref s, uint16 *_Nullable buf, size_t wsz)
{
    size_t bufidx = 0;
    int32 codepoint = 0;
    if (!STR_CHECK_VALID(s) || !strValidUTF8(s))
        return 0;

    striter it;
    uint32 len = _strFastLen(s);
    striBorrow(&it, s);

    while (len > 0) {
        uint32 seqlen = _strUTF8Decode(&it, &codepoint);
        devAssert(seqlen > 0);          // should be impossible to get here with invalid UTF-8
        len -= seqlen;

        if (codepoint < 0x10000) {
            // BMP
            if (buf) {
                if (bufidx + 1 > wsz)
                    return 0;
                buf[bufidx] = (uint16)codepoint;
            }
            bufidx++;
        } else {
            if (buf) {
                if (bufidx + 2 > wsz)
                    return 0;
                codepoint -= 0x10000;
                buf[bufidx] = 0xd800 | ((codepoint >> 10) & 0x3ff);
                buf[bufidx + 1] = 0xdc00 | (codepoint & 0x3ff);
            }
            bufidx += 2;
        }
    }

    // null terminator
    if (buf) {
        if (bufidx + 1 > wsz)
            return 0;
        buf[bufidx] = 0;
    }
    bufidx++;

    return bufidx;
}

_Use_decl_annotations_
bool strFromUTF16(strhandle o, const uint16 *_Nonnull buf, size_t wsz)
{
    bool surrogate = false;
    int nexpand = 1;
    int32 codepoint;

    // start out assuming it's ASCII text to minimize reallocation
    uint32 osz = (uint32)wsz + 5, olen = 0;
    _strReset(o, (uint32)osz);

    for (uint32 i = 0; i < (uint32)wsz; i++) {
        if (buf[i] < 0xd800 || buf[i] >= 0xe000) {
            // BMP
            codepoint = buf[i];
        } else if (!surrogate && (buf[i] & 0xfc00) == 0xd800) {
            surrogate = true;
            codepoint = ((int32)buf[i] & 0x3ff) << 10;
            continue;
        } else if (surrogate && (buf[i] & 0xfc00) == 0xdc00) {
            surrogate = false;
            codepoint |= (int32)buf[i] & 0x3ff;
            codepoint += 0x10000;
            if (codepoint > 0x10ffff || (codepoint >= 0xd800 && codepoint <= 0xdfff))
                goto fail;      // not legal unicode!
        } else {
            goto fail;          // invalid UTF-16!
        }

        if (codepoint == 0)
            break;              // hit NULL before end of buffer, stop string here

        if (olen + 5 > osz) {
            // expand by more each time to not reallocate a ton if this string has
            // lots of multibyte sequences
            osz += 16 * nexpand;
            nexpand++;
            _strResize(o, osz, false);
        }

        olen += _strUTF8Encode(_strBuffer(*o) + olen, codepoint);
    }

    *(_strBuffer(*o) + olen) = 0;
    _strSetLen(*o, olen);

    return true;

fail:
    strClear(o);
    return false;
}

_Use_decl_annotations_
uint16 *_Nullable strToUTF16A(strref s)
{
    size_t sz = strToUTF16(s, NULL, 0);
    if (sz == 0)
        return NULL;

    uint16 *ret = xaAlloc(sz * sizeof(uint16));
    strToUTF16(s, ret, sz);
    return ret;
}

_Use_decl_annotations_
uint16 *_Nullable strToUTF16S(strref s)
{
    size_t sz = strToUTF16(s, NULL, 0);
    if (sz == 0)
        return NULL;

    uint16 *ret = scratchGet(sz * sizeof(uint16));
    strToUTF16(s, ret, sz);
    return ret;
}
