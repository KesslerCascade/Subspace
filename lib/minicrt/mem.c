#include "minicrt.h"

#include <ctype.h>

#pragma function(memcpy)
void* memcpy(void* dst, const void* src, unsigned int count)
{
    void* ret = dst;

    while (count--) {
        *(char*)dst = *(char*)src;
        dst         = (char*)dst + 1;
        src         = (char*)src + 1;
    }

    return ret;
}

#pragma function(memcmp)
int memcmp(const void* s1, const void* s2, unsigned int count)
{
    unsigned char *u1 = (unsigned char*)s1, *u2 = (unsigned char*)s2;

    for (; count--; u1++, u2++) {
        if (*u1 != *u2)
            return *u1 - *u2;
    }
    return 0;
}

int _memicmp(const void* s1, const void* s2, unsigned int count)
{
    unsigned char *u1 = (unsigned char*)s1, *u2 = (unsigned char*)s2;

    for (; count--; u1++, u2++) {
        unsigned char lu1 = __ascii_toupper(*u1);
        unsigned char lu2 = __ascii_toupper(*u2);
        if (lu1 != lu2)
            return lu1 - lu2;
    }
    return 0;
}

#pragma function(memset)
void* memset(void* s, int c, unsigned int count)
{
    unsigned char* p  = s;
    unsigned int fill = (c << 24) | (c << 16) | (c << 8) | c;
    for (; count >= 4; p += 4, count -= 4) {
        *(unsigned int*)p = fill;
    }
    for (; count > 0; ++p, --count) {
        *p = c;
    }
    return s;
}

#define wsize sizeof(int)
#define wmask (wsize - 1)

void* memmove(void* dst0, const void* src0, unsigned int length)
{
    char* dst       = dst0;
    const char* src = src0;
    size_t t;

    if (length == 0 || dst == src) /* nothing to do */
        goto done;

    /*
     * Macros: loop-t-times; and loop-t-times, t>0
     */
#define TLOOP(s) \
    if (t)       \
    TLOOP1(s)
#define TLOOP1(s) \
    do {          \
        s;        \
    } while (--t)

    if ((unsigned long)dst < (unsigned long)src) {
        /*
         * Copy forward.
         */
        t = (uintptr_t)src; /* only need low bits */
        if ((t | (uintptr_t)dst) & wmask) {
            /*
             * Try to align operands.  This cannot be done
             * unless the low bits match.
             */
            if ((t ^ (uintptr_t)dst) & wmask || length < wsize)
                t = length;
            else
                t = wsize - (t & wmask);
            length -= t;
            TLOOP1(*dst++ = *src++);
        }
        /*
         * Copy whole words, then mop up any trailing bytes.
         */
        t = length / wsize;
        TLOOP(*(int*)dst = *(int*)src; src += wsize; dst += wsize);
        t = length & wmask;
        TLOOP(*dst++ = *src++);
    } else {
        /*
         * Copy backwards.  Otherwise essentially the same.
         * Alignment works as before, except that it takes
         * (t&wmask) bytes to align, not wsize-(t&wmask).
         */
        src += length;
        dst += length;
        t = (uintptr_t)src;
        if ((t | (uintptr_t)dst) & wmask) {
            if ((t ^ (uintptr_t)dst) & wmask || length <= wsize)
                t = length;
            else
                t &= wmask;
            length -= t;
            TLOOP1(*--dst = *--src);
        }
        t = length / wsize;
        TLOOP(src -= wsize; dst -= wsize; *(int*)dst = *(int*)src);
        t = length & wmask;
        TLOOP(*--dst = *--src);
    }
done:
    return (dst0);
}
