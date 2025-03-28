#include <stdint.h>
#include <string.h>
#include <cx/cx.h>
#include <cx/platform/base.h>
#include "xalloc.h"
#include "cstrutil.h"

// Why does this file exist? Because Microsoft is stupid and renamed strdup,
// since it's technically "non-standard". As it's not part of ISO and
// compatibility is a pain, we'll use an optimized replacement instead
// to sidestep the portability issues.

// memset, memcpy and memmove should be used as-is from string.h as the
// compiler intrinsics for those will be faster than anything that could be
// supplied here.

_Ret_z_ char *cstrDup(_In_z_ const char *src)
{
    if (!src)
        return NULL;

    size_t len = cstrLen(src) + 1;
    char *ret = xaAlloc(len);

    memcpy(ret, src, len);
    return ret;
}

size_t cstrLenw(_In_z_ const unsigned short *s)
{
    const unsigned short *p = s;
    while (*p)
        p++;
    return p - s;
}

_Ret_z_ unsigned short *cstrDupw(_In_z_ const unsigned short *src)
{
    if (!src)
        return NULL;

    size_t len = (cstrLenw(src) + 1) * 2;
    unsigned short *ret = xaAlloc(len);

    memcpy(ret, src, len);
    return ret;
}

// Below this point is efficient strlen function borrowed from FreeBSD

/*-
 * Copyright (c) 2009, 2010 Xin LI <delphij@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Portable strlen() for 32-bit and 64-bit systems.
 *
 * Rationale: it is generally much more efficient to do word length
 * operations and avoid branches on modern computer systems, as
 * compared to byte-length operations with a lot of branches.
 *
 * The expression:
 *
 *      ((x - 0x01....01) & ~x & 0x80....80)
 *
 * would evaluate to a non-zero value iff any of the bytes in the
 * original word is zero.
 *
 * On multi-issue processors, we can divide the above expression into:
 *      a)  (x - 0x01....01)
 *      b) (~x & 0x80....80)
 *      c) a & b
 *
 * Where, a) and b) can be partially computed in parallel.
 *
 * The algorithm above is found on "Hacker's Delight" by
 * Henry S. Warren, Jr.
 */

/* Magic numbers for the algorithm */
#if defined(_32BIT)
static const uintptr_t mask01 = 0x01010101;
static const uintptr_t mask80 = 0x80808080;
#elif defined(_64BIT)
static const uintptr_t mask01 = 0x0101010101010101;
static const uintptr_t mask80 = 0x8080808080808080;
#else
#error Unsupported word size
#endif

#define LONGPTR_MASK (sizeof(uintptr_t) - 1)

/*
 * Helper macro to return string length if we caught the zero
 * byte.
 */
#define testbyte(x)                             \
        do {                                    \
                if (p[x] == '\0')               \
                    return (p - str + x);       \
        } while (0)

size_t cstrLen(_In_z_ const char *str)
{
    const char *p;
    const uintptr_t *lp;
    intptr_t va, vb;

    /*
     * Before trying the hard (unaligned byte-by-byte access) way
     * to figure out whether there is a nul character, try to see
     * if there is a nul character is within this accessible word
     * first.
     *
     * p and (p & ~LONGPTR_MASK) must be equally accessible since
     * they always fall in the same memory page, as long as page
     * boundaries is integral multiple of word size.
     */
    lp = (const uintptr_t *)((uintptr_t)str & ~LONGPTR_MASK);
    va = (*lp - mask01);
    vb = ((~*lp) & mask80);
    lp++;
    if (va & vb)
            /* Check if we have \0 in the first part */
        for (p = str; p < (const char *)lp; p++)
            if (*p == '\0')
                return (p - str);

/* Scan the rest of the string using word sized operation */
    for (; ; lp++) {
        va = (*lp - mask01);
        vb = ((~*lp) & mask80);
        if (va & vb) {
            p = (const char *)(lp);
            testbyte(0);
            testbyte(1);
            testbyte(2);
            testbyte(3);
#if defined(_64BIT)
            testbyte(4);
            testbyte(5);
            testbyte(6);
            testbyte(7);
#endif
        }
    }

    /* NOTREACHED */
    return (0);
}

// case insensitive compare since it's not consistently available
int cstrCmpi(_In_z_ const char *s1, _In_z_ const char *s2)
{
    const uint8
        *us1 = (const uint8*)s1,
        *us2 = (const uint8*)s2;

    while (tolower(*us1) == tolower(*us2++))
        if (*us1++ == '\0')
            return (0);
    return (tolower(*us1) - tolower(*--us2));
}
