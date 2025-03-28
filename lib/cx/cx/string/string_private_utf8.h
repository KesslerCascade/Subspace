#pragma once

_meta_inline _Pure uint32 _strUTF8SeqLen(uint8 u)
{
    // single byte encoding aka ASCII
    if (u < 0x80)
        return 1;

    if (u >= 0x80 && u <= 0xbf)
        return 0;       // continuation byte, not valid here!
    else if (u == 0xc0 || u == 0xc1)
        return 0;       // overlong encoding of code point < 0x80
    else if (u >= 0xc2 && u <= 0xdf)
        return 2;
    else if (u >= 0xe0 && u <= 0xef)
        return 3;
    else if (u >= 0xf0 && u <= 0xf4)
        return 4;

    return 0;
}

_meta_inline bool _strUTF8DecodeSeq(striter *_Nonnull it, uint32 len, uint8 ch, int32 *_Nullable codepoint)
{
    int32 ret = 0;

    if (len == 2)
        ret = ch & 0x1f;
    else if (len == 3)
        ret = ch & 0x0f;
    else if (len == 4)
        ret = ch & 0x07;
    else
        return false;

    for (; len > 1; --len) {
        if (!striChar(it, (uint8*)&ch))
            return false;

        if (ch < 0x80 || ch > 0xbf)
            return false;           // continuation byte must follow

        ret = (ret << 6) | (ch & 0x3f);
    }

    if (ret > 0x10ffff ||                       // outside unicode range
        (ret >= 0xd800 && ret <= 0xdfff) ||     // UTF-16 surrogate pairs
        (len == 2 && ret < 0x80) ||             // overlong encodings
        (len == 3 && ret < 0x800) ||
        (len == 4 && ret < 0x10000))
        return false;

    if (codepoint)
        *codepoint = ret;

    return true;
}

_meta_inline uint32 _strUTF8Decode(striter *_Nonnull it, int32 *_Nullable codepoint)
{
    uint8 first;
    if (!striChar(it, (uint8*)&first))
        return 0;

    uint32 len = _strUTF8SeqLen(first);

    if (len == 1) {
        if (codepoint)
            *codepoint = first;
        return 1;
    }

    if (_strUTF8DecodeSeq(it, len, first, codepoint))
        return len;
    return 0;
}

_meta_inline uint32 _strUTF8Encode(uint8 *_Nonnull buffer, int32 codepoint)
{
    if (codepoint < 0)
        return 0;
    else if (codepoint < 0x80) {
        buffer[0] = (uint8)codepoint;
        return 1;
    } else if (codepoint < 0x800) {
        buffer[0] = 0xc0 | ((codepoint & 0x7c0) >> 6);
        buffer[1] = 0x80 | ((codepoint & 0x03f));
        return 2;
    } else if (codepoint < 0x10000) {
        buffer[0] = 0xe0 | ((codepoint & 0xf000) >> 12);
        buffer[1] = 0x80 | ((codepoint & 0x0fc0) >> 6);
        buffer[2] = 0x80 | ((codepoint & 0x003f));
        return 3;
    } else if (codepoint < 0x10ffff) {
        buffer[0] = 0xf0 | ((codepoint & 0x1c0000) >> 18);
        buffer[1] = 0x80 | ((codepoint & 0x03f000) >> 12);
        buffer[2] = 0x80 | ((codepoint & 0x000fc0) >> 6);
        buffer[3] = 0x80 | ((codepoint & 0x00003f));
        return 4;
    }

    return 0;
}
