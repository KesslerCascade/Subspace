#include "minicrt.h"

#include <ctype.h>
#include <limits.h>

int _minicrt_strcmp(const char* str1, const char* str2)
{
    unsigned char s1;
    unsigned char s2;
    do {
        s1 = *str1++;
        s2 = *str2++;
        if (s1 == 0)
            break;
    } while (s1 == s2);
    return (s1 < s2) ? -1 : (s1 > s2);
}

int _minicrt_strncmp(const char* s1, const char* s2, unsigned int n)
{
    for (; n > 0; s1++, s2++, --n)
        if (*s1 != *s2)
            return ((*(unsigned char*)s1 < *(unsigned char*)s2) ? -1 : +1);
        else if (*s1 == '\0')
            return 0;
    return 0;
}

int _minicrt_stricmp(const char* str1, const char* str2)
{
    unsigned char s1;
    unsigned char s2;
    do {
        s1 = __ascii_toupper(*str1);
        s2 = __ascii_toupper(*str2);
        str1++, str2++;
    } while (s1 != 0 && s1 == s2);
    return (s1 < s2) ? -1 : (s1 > s2);
}

int _minicrt_strnicmp(const char* str1, const char* str2, unsigned int count)
{
    unsigned char s1;
    unsigned char s2;
    do {
        if ((count--) == 0)
            return 0;
        s1 = __ascii_toupper(*str1);
        s2 = __ascii_toupper(*str2);
        str1++, str2++;
    } while (s1 != 0 && s1 == s2);
    return (s1 < s2) ? -1 : (s1 > s2);
}

char* _minicrt_strcat(char* d, const char* src)
{
    char* ret = d;
    for (; *d; ++d);
    while ((*d++ = *src++));
    return ret;
}

char* _minicrt_strcpy(char* d, const char* src)
{
    char* ret = d;
    for (; (*d = *src); ++d, ++src);
    return ret;
}

char* _minicrt_strlwr(char* str)
{
    char* s = str;
    while (*s) {
        if (*s >= 'A' && *s <= 'Z')
            *s |= 32;
        ++s;
    }
    return str;
}

char* _minicrt_strtok_r(char* s, const char* delim, char** last)
{
    char *spanp, *tok;
    int c, sc;

    if (s == NULL && (s = *last) == NULL)
        return (NULL);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
cont:
    c = *s++;
    for (spanp = (char*)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    }

    if (c == 0) { /* no non-delimiter characters */
        *last = NULL;
        return (NULL);
    }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;) {
        c     = *s++;
        spanp = (char*)delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = '\0';
                *last = s;
                return (tok);
            }
        } while (sc != 0);
    }
    /* NOTREACHED */
}

char* _minicrt_strtok(char* s, const char* delim)
{
    static char* last;

    return (strtok_r(s, delim, &last));
}

int _minicrt_atoi(const char* str)
{
    return (int)_minicrt_strtol(str, (char**)NULL, 10);
}

static __inline int _isspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\r' || c == '\f');
}

long _minicrt_strtol(const char* nptr, char** endptr, int base)
{
    const char* s;
    unsigned long acc;
    char c;
    unsigned long cutoff;
    int neg, any, cutlim;

    /*
     * Skip white space and pick up leading +/- sign if any.
     * If base is 0, allow 0x for hex and 0 for octal, else
     * assume decimal; if base is already 16, allow 0x.
     */
    s = nptr;
    do {
        c = *s++;
    } while (_isspace((unsigned char)c));
    if (c == '-') {
        neg = 1;
        c   = *s++;
    } else {
        neg = 0;
        if (c == '+')
            c = *s++;
    }
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X') &&
        ((s[1] >= '0' && s[1] <= '9') || (s[1] >= 'A' && s[1] <= 'F') ||
         (s[1] >= 'a' && s[1] <= 'f'))) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;
    acc = any = 0;
    if (base < 2 || base > 36)
        goto noconv;

    /*
     * Compute the cutoff value between legal numbers and illegal
     * numbers.  That is the largest legal value, divided by the
     * base.  An input number that is greater than this value, if
     * followed by a legal input character, is too big.  One that
     * is equal to this value may be valid or not; the limit
     * between valid and invalid numbers is then based on the last
     * digit.  For instance, if the range for longs is
     * [-2147483648..2147483647] and the input base is 10,
     * cutoff will be set to 214748364 and cutlim to either
     * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
     * a value > 214748364, or equal but the next digit is > 7 (or 8),
     * the number is too big, and we will return a range error.
     *
     * Set 'any' if any `digits' consumed; make it negative to indicate
     * overflow.
     */
    cutoff = neg ? (unsigned long)-(LONG_MIN + LONG_MAX) + LONG_MAX : LONG_MAX;
    cutlim = cutoff % base;
    cutoff /= base;
    for (;; c = *s++) {
        if (c >= '0' && c <= '9')
            c -= '0';
        else if (c >= 'A' && c <= 'Z')
            c -= 'A' - 10;
        else if (c >= 'a' && c <= 'z')
            c -= 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
        //        errno = ERANGE;
    } else if (!any) {
noconv:;
        //        errno = EINVAL;
    } else if (neg)
        acc = -(long)acc;
    if (endptr != NULL)
        *endptr = (char*)(any ? s - 1 : nptr);
    return (acc);
}

static const unsigned long mask01 = 0x01010101;
static const unsigned long mask80 = 0x80808080;

#define LONGPTR_MASK (sizeof(long) - 1)
#define testbyte(x)               \
    do {                          \
        if (p[x] == '\0')         \
            return (p - str + x); \
    } while (0)

unsigned int _minicrt_strlen(const char* str)
{
    const char* p;
    const unsigned long* lp;
    long va, vb;

    lp = (const unsigned long*)((unsigned int)str & ~LONGPTR_MASK);
    va = (*lp - mask01);
    vb = ((~*lp) & mask80);
    lp++;
    if (va & vb)
        /* Check if we have \0 in the first part */
        for (p = str; p < (const char*)lp; p++)
            if (*p == '\0')
                return (p - str);

    /* Scan the rest of the string using word sized operation */
    for (;; lp++) {
        va = (*lp - mask01);
        vb = ((~*lp) & mask80);
        if (va & vb) {
            p = (const char*)(lp);
            testbyte(0);
            testbyte(1);
            testbyte(2);
            testbyte(3);
        }
    }
}

char* _minicrt_strchr(const char* p, int ch)
{
    for (; *p; ++p) {
        if (*p == ch)
            return (char*)p;
    }
    return NULL;
}


char* _minicrt_strrchr(const char* p, int ch)
{
    char* save;
    char c;

    c = ch;
    for (save = NULL;; ++p) {
        if (*p == c)
            save = (char*)p;
        if (*p == '\0')
            return (save);
    }
    /* NOTREACHED */
}
