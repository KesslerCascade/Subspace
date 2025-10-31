#include "digest_private.h"

// Adapted from https://github.com/clausecker/digest/

/* Copyright (c) 2013, Robert Clausecker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE. */

enum {
    SHA1_K0 = 0x5A827999,
    SHA1_K1 = 0x6ED9EBA1,
    SHA1_K2 = 0x8F1BBCDC,
    SHA1_K3 = 0xCA62C1D6,
};

static const uint32 sha1_init_state[] = { 0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0 };

void _sha1Init(Digest* digest)
{
    memcpy(digest->state, sha1_init_state, sizeof(sha1_init_state));
}

void _sha1Block(Digest* digest)
{
    uint32* s = digest->state;
    uint8* p  = digest->buffer;
    uint32 a = s[0], b = s[1], c = s[2], d = s[3], e = s[4];
    uint32 w[16], f, a5, b30, t, tmp, j;
    int i;

    for (i = 0; i < 16; i++) {
        j    = i * 4;
        w[i] = (uint32)p[j] << 24 | (uint32)p[j + 1] << 16 | (uint32)p[j + 2] << 8 |
            (uint32)p[j + 3];
    }

    i = 0;

    for (; i < 16; i++) {
        f   = (b & c) | (~b & d);
        a5  = a << 5 | a >> (32 - 5);
        b30 = b << 30 | b >> (32 - 30);
        t   = a5 + f + e + w[i & 0xf] + SHA1_K0;
        e   = d;
        d   = c;
        c   = b30;
        b   = a;
        a   = t;
    }

    for (; i < 20; i++) {
        tmp        = w[(i - 3) & 0xf] ^ w[(i - 8) & 0xf] ^ w[(i - 14) & 0xf] ^ w[i & 0xf];
        w[i & 0xf] = tmp << 1 | tmp >> (32 - 1);

        f   = (b & c) | (~b & d);
        a5  = a << 5 | a >> (32 - 5);
        b30 = b << 30 | b >> (32 - 30);
        t   = a5 + f + e + w[i & 0xf] + SHA1_K0;
        e   = d;
        d   = c;
        c   = b30;
        b   = a;
        a   = t;
    }

    for (; i < 40; i++) {
        tmp        = w[(i - 3) & 0xf] ^ w[(i - 8) & 0xf] ^ w[(i - 14) & 0xf] ^ w[i & 0xf];
        w[i & 0xf] = tmp << 1 | tmp >> (32 - 1);

        f   = b ^ c ^ d;
        a5  = a << 5 | a >> (32 - 5);
        b30 = b << 30 | b >> (32 - 30);
        t   = a5 + f + e + w[i & 0xf] + SHA1_K1;
        e   = d;
        d   = c;
        c   = b30;
        b   = a;
        a   = t;
    }

    for (; i < 60; i++) {
        tmp        = w[(i - 3) & 0xf] ^ w[(i - 8) & 0xf] ^ w[(i - 14) & 0xf] ^ w[i & 0xf];
        w[i & 0xf] = tmp << 1 | tmp >> (32 - 1);

        f   = ((b | c) & d) | (b & c);
        a5  = a << 5 | a >> (32 - 5);
        b30 = b << 30 | b >> (32 - 30);
        t   = a5 + f + e + w[i & 0xf] + SHA1_K2;
        e   = d;
        d   = c;
        c   = b30;
        b   = a;
        a   = t;
    }

    for (; i < 80; i++) {
        tmp        = w[(i - 3) & 0xf] ^ w[(i - 8) & 0xf] ^ w[(i - 14) & 0xf] ^ w[i & 0xf];
        w[i & 0xf] = tmp << 1 | tmp >> (32 - 1);

        f   = b ^ c ^ d;
        a5  = a << 5 | a >> (32 - 5);
        b30 = b << 30 | b >> (32 - 30);
        t   = a5 + f + e + w[i & 0xf] + SHA1_K3;
        e   = d;
        d   = c;
        c   = b30;
        b   = a;
        a   = t;
    }

    s[0] += a;
    s[1] += b;
    s[2] += c;
    s[3] += d;
    s[4] += e;
}
