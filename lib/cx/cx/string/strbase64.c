#include "strencoding.h"
#include "string_private.h"

static const char _base64_invcharmap[256] = {
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,62, 65,62,65,63,
    52,53,54,55, 56,57,58,59, 60,61,65,65, 65,64,65,65,
    65, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,65, 65,65,65,63,
    65,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65,
    65,65,65,65, 65,65,65,65, 65,65,65,65, 65,65,65,65
};

/** map of all 6-bit values to their corresponding byte
in the base64 alphabet. Padding char is the value '64' */
static const char _base64_charmap[65] = {
    'A','B','C','D', 'E','F','G','H',
    'I','J','K','L', 'M','N','O','P',
    'Q','R','S','T', 'U','V','W','X',
    'Y','Z','a','b', 'c','d','e','f',
    'g','h','i','j', 'k','l','m','n',
    'o','p','q','r', 's','t','u','v',
    'w','x','y','z', '0','1','2','3',
    '4','5','6','7', '8','9','+','/',
    '='
};

static const char _base64_charmap_urlsafe[65] = {
    'A','B','C','D', 'E','F','G','H',
    'I','J','K','L', 'M','N','O','P',
    'Q','R','S','T', 'U','V','W','X',
    'Y','Z','a','b', 'c','d','e','f',
    'g','h','i','j', 'k','l','m','n',
    'o','p','q','r', 's','t','u','v',
    'w','x','y','z', '0','1','2','3',
    '4','5','6','7', '8','9','-','_',
    '='
};

static uint32 b64EncodedLen(uint32 bufsz)
{
    // encoded steam is 4 bytes for every three, rounded up
    return ((bufsz + 2) / 3) << 2;
}

_Use_decl_annotations_
bool strB64Encode(strhandle out, const uint8 *_Nonnull buf, uint32 bufsz, bool urlsafe)
{
    const char *charmap = urlsafe ? _base64_charmap_urlsafe : _base64_charmap;
    uint32 word, hextet, i;
    uint8 *c;

    uint32 elen = b64EncodedLen(bufsz);
    _strReset(out, elen);
    c = strBuffer(out, elen);

    // loop over data, turning every 3 bytes into 4 characters
    for (i = 0; i < bufsz - 2; i += 3) {
        word = buf[i] << 16 | buf[i + 1] << 8 | buf[i + 2];
        hextet = (word & 0x00FC0000) >> 18;
        *c++ = charmap[hextet];
        hextet = (word & 0x0003F000) >> 12;
        *c++ = charmap[hextet];
        hextet = (word & 0x00000FC0) >> 6;
        *c++ = charmap[hextet];
        hextet = (word & 0x000003F);
        *c++ = charmap[hextet];
    }
    // zero, one or two bytes left
    switch (bufsz - i) {
    case 0:
        break;
    case 1:
        hextet = (buf[bufsz - 1] & 0xFC) >> 2;
        *c++ = charmap[hextet];
        hextet = (buf[bufsz - 1] & 0x03) << 4;
        *c++ = charmap[hextet];
        *c++ = charmap[64]; /* pad */
        *c++ = charmap[64]; /* pad */
        break;
    case 2:
        hextet = (buf[bufsz - 2] & 0xFC) >> 2;
        *c++ = charmap[hextet];
        hextet = ((buf[bufsz - 2] & 0x03) << 4) |
            ((buf[bufsz - 1] & 0xF0) >> 4);
        *c++ = charmap[hextet];
        hextet = (buf[bufsz - 1] & 0x0F) << 2;
        *c++ = charmap[hextet];
        *c++ = charmap[64]; /* pad */
        break;
    }

    return true;
}

static uint32 b64DecodedSize(_In_ strref str)
{
    uint32 len = strLen(str);
    int nudge;
    int c;

    // count the padding characters for the remainder
    nudge = -1;
    c = _base64_invcharmap[(uint8)_strFastChar(str, len - 1)];
    if (c < 64) nudge = 0;
    else if (c == 64) {
        c = _base64_invcharmap[(uint8)_strFastChar(str, len - 2)];
        if (c < 64) nudge = 1;
        else if (c == 64) {
            c = _base64_invcharmap[(uint8)_strFastChar(str, len - 3)];
            if (c < 64) nudge = 2;
        }
    }
    if (nudge < 0) return 0;    // reject bad coding

    // decoded steam is 3 bytes for every four
    return 3 * (len >> 2) - nudge;
}

_Use_decl_annotations_
uint32 strB64Decode(strref str, uint8 *_Nullable d, uint32 bufsz)
{
    uint32 dlen;
    uint32 word, len, i;
    uint32 hextet = 0;

    len = strLen(str);
    // len must be a multiple of 4
    if (!str || (len & 0x03)) return 0;

    dlen = b64DecodedSize(str);
    if (!d)
        return dlen;

    if (dlen > bufsz)
        return 0;

    striter sti;
    striBorrow(&sti, str);

    /* loop over each set of 4 characters, decoding 3 bytes */
    uint8 ch = 0;
    for (i = 0; i < len - 3; i += 4) {
        // return valid of striChar can be ignored since we already checked length
        (void)striChar(&sti, &ch);
        hextet = _base64_invcharmap[ch];
        if (hextet & 0xC0) break;
        word = hextet << 18;
        (void)striChar(&sti, &ch);
        hextet = _base64_invcharmap[ch];
        if (hextet & 0xC0) break;
        word |= hextet << 12;
        (void)striChar(&sti, &ch);
        hextet = _base64_invcharmap[ch];
        if (hextet & 0xC0) break;
        word |= hextet << 6;
        (void)striChar(&sti, &ch);
        hextet = _base64_invcharmap[ch];
        if (hextet & 0xC0) break;
        word |= hextet;
        *d++ = (word & 0x00FF0000) >> 16;
        *d++ = (word & 0x0000FF00) >> 8;
        *d++ = (word & 0x000000FF);
    }
    if (hextet > 64) goto _base64_decode_error;
    /* handle the remainder */
    switch (dlen % 3) {
    case 0:
        /* nothing to do */
        break;
    case 1:
        /* redo the last quartet, checking for correctness */
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 4)];
        if (hextet & 0xC0) goto _base64_decode_error;
        word = hextet << 2;
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 3)];
        if (hextet & 0xC0) goto _base64_decode_error;
        word |= hextet >> 4;
        *d++ = word & 0xFF;
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 2)];
        if (hextet != 64) goto _base64_decode_error;
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 1)];
        if (hextet != 64) goto _base64_decode_error;
        break;
    case 2:
        /* redo the last quartet, checking for correctness */
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 4)];
        if (hextet & 0xC0) goto _base64_decode_error;
        word = hextet << 10;
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 3)];
        if (hextet & 0xC0) goto _base64_decode_error;
        word |= hextet << 4;
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 2)];
        if (hextet & 0xC0) goto _base64_decode_error;
        word |= hextet >> 2;
        *d++ = (word & 0xFF00) >> 8;
        *d++ = (word & 0x00FF);
        hextet = _base64_invcharmap[(uint8)_strFastChar(str, len - 1)];
        if (hextet != 64) goto _base64_decode_error;
        break;
    }
    if (dlen + 1 <= bufsz)
        *d = '\0';          // add an extra null if there's room

    return dlen;

_base64_decode_error:
    // invalid character; abort decoding!
    return false;
}
