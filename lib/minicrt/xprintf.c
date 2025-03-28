/*------------------------------------------------------------------------/
/  Universal string handler for user console interface
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

#include "minicrt.h"
#include <stdarg.h>

#define _CR_CRLF 1

static void (*xfunc_out)(unsigned char) = 0;    /* Pointer to the output stream */
static char *outptr = 0;
static char *outptrend = 0;
static wchar_t *woutptr = 0;
static wchar_t *woutptrend = 0;

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xputc (char c)
{
    if (_CR_CRLF && c == '\n') xputc('\r');     /* CR -> CRLF */

    if (outptr && (!outptrend || outptr < outptrend)) {
        *outptr++ = (unsigned char)c;
        return;
    }

    if (xfunc_out) xfunc_out((unsigned char)c);
}

void wxputc(wchar_t c)
{
    if (_CR_CRLF && c == '\n') xputc('\r');     /* CR -> CRLF */

    if (woutptr && (!woutptrend || woutptr < woutptrend)) {
        *woutptr++ = (unsigned char)c;
        return;
    }

//    if (xfunc_out) xfunc_out((unsigned char)c);
}


/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void xputs (                    /* Put a string to the default device */
    const char* str             /* Pointer to the string */
)
{
    while (*str)
        xputc(*str++);
}

void wxputs(                    /* Put a string to the default device */
           const wchar_t* str   /* Pointer to the string */
           )
{
    while (*str)
        wxputc(*str++);
}

void wxputss(                   /* Put a string to the default device */
           const char* str      /* Pointer to the string */
           )
{
    while (*str)
        wxputc(*str++);
}
#if 0
void xfputs (                   /* Put a string to the specified device */
    void(*func)(unsigned char), /* Pointer to the output function */
    const char* str             /* Pointer to the string */
)
{
    void (*pf)(unsigned char);


    pf = xfunc_out;     /* Save current output device */
    xfunc_out = func;   /* Switch output to specified device */
    while (*str)        /* Put the string */
        xputc(*str++);
    xfunc_out = pf;     /* Restore output device */
}
#endif


/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);            "1234"
    xprintf("%6d,%3d%%", -200, 5);  "  -200,  5%"
    xprintf("%-6u", 100);           "100   "
    xprintf("%ld", 12345678L);      "12345678"
    xprintf("%04x", 0xA3);          "00a3"
    xprintf("%08LX", 0x123ABC);     "00123ABC"
    xprintf("%016b", 0x550F);       "0101010100001111"
    xprintf("%s", "String");        "String"
    xprintf("%-4s", "abc");         "abc "
    xprintf("%4s", "abc");          " abc"
    xprintf("%c", 'a');             "a"
    xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/

static void _minicrt_xvprintf(const char* fmt, /* Pointer to the format string */
                              va_list arp      /* Pointer to arguments */
)
{
    unsigned int r, i, j, w, f;
    unsigned long v;
    char s[16], c, d, *p;


    for (;;) {
        c = *fmt++;                 /* Get a char */
        if (!c) break;              /* End of format? */
        if (c != '%') {             /* Pass through it if not a % sequense */
            xputc(c); continue;
        }
        f = 0;
        c = *fmt++;                 /* Get first char of the sequense */
        if (c == '0') {             /* Flag: '0' padded */
            f = 1; c = *fmt++;
        } else {
            if (c == '-') {         /* Flag: left justified */
                f = 2; c = *fmt++;
            }
        }
        for (w = 0; c >= '0' && c <= '9'; c = *fmt++)   /* Minimum width */
            w = w * 10 + c - '0';
        if (c == 'l' || c == 'L') { /* Prefix: Size is long int */
            f |= 4; c = *fmt++;
        }
        if (!c) break;              /* End of format? */
        d = c;
        if (d >= 'a') d -= 0x20;
        switch (d) {                /* Type is... */
        case 'S' :                  /* String */
            p = va_arg(arp, char*);
            for (j = 0; p[j]; j++) ;
            while (!(f & 2) && j++ < w) xputc(' ');
            xputs(p);
            while (j++ < w) xputc(' ');
            continue;
        case 'C' :                  /* Character */
            xputc((char)va_arg(arp, int)); continue;
        case 'B' :                  /* Binary */
            r = 2; break;
        case 'O' :                  /* Octal */
            r = 8; break;
        case 'D' :                  /* Signed decimal */
        case 'U' :                  /* Unsigned decimal */
            r = 10; break;
        case 'X' :                  /* Hexdecimal */
            r = 16; break;
        default:                    /* Unknown type (passthrough) */
            xputc(c); continue;
        }

        /* Get an argument and put it in numeral */
        v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
        if (d == 'D' && (v & 0x80000000)) {
            v = 0 - v;
            f |= 8;
        }
        i = 0;
        do {
            d = (char)(v % r); v /= r;
            if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
            s[i++] = d + '0';
        } while (v && i < sizeof(s));
        if (f & 8) s[i++] = '-';
        j = i; d = (f & 1) ? '0' : ' ';
        while (!(f & 2) && j++ < w) xputc(d);
        do xputc(s[--i]); while(i);
        while (j++ < w) xputc(' ');
    }
}

static void _minicrt_wxvprintf(const wchar_t* fmt, /* Pointer to the format string */
                               va_list arp         /* Pointer to arguments */
)
{
    unsigned int r, i, j, w, f;
    unsigned long v;
    wchar_t s[16], c, d, *p;
    char *sp;

    for (;;) {
        c = *fmt++;                 /* Get a char */
        if (!c) break;              /* End of format? */
        if (c != '%') {             /* Pass through it if not a % sequense */
            wxputc(c); continue;
        }
        f = 0;
        c = *fmt++;                 /* Get first char of the sequense */
        if (c == '0') {             /* Flag: '0' padded */
            f = 1; c = *fmt++;
        } else {
            if (c == '-') {         /* Flag: left justified */
                f = 2; c = *fmt++;
            }
        }
        for (w = 0; c >= '0' && c <= '9'; c = *fmt++)   /* Minimum width */
            w = w * 10 + c - '0';
        if (c == 'l' || c == 'L') { /* Prefix: Size is long int */
            f |= 4; c = *fmt++;
        }
        if (!c) break;              /* End of format? */
        d = c;
        if (d >= 'a') d -= 0x20;
        switch (c) {                /* Type is... */
        case 's':                   /* String */
            p = va_arg(arp, wchar_t*);
            for (j = 0; p[j]; j++);
            while (!(f & 2) && j++ < w) wxputc(' ');
            wxputs(p);
            while (j++ < w) wxputc(' ');
            continue;
        case 'S':                   /* 8-bit String */
            sp = va_arg(arp, char*);
            for (j = 0; sp[j]; j++);
            while (!(f & 2) && j++ < w) wxputc(' ');
            wxputss(sp);
            while (j++ < w) wxputc(' ');
            continue;
        case 'c':                   /* Character */
        case 'C':                   /* Character */
            wxputc((char)va_arg(arp, int)); continue;
        case 'b':                   /* Binary */
        case 'B':                   /* Binary */
            r = 2; break;
        case 'o':                   /* Octal */
        case 'O':                   /* Octal */
            r = 8; break;
        case 'd':                   /* Signed decimal */
        case 'D':                   /* Signed decimal */
        case 'u':                   /* Unsigned decimal */
        case 'U':                   /* Unsigned decimal */
            r = 10; break;
        case 'x':                   /* Hexdecimal */
        case 'X':                   /* Hexdecimal */
            r = 16; break;
        default:                    /* Unknown type (passthrough) */
            wxputc(c); continue;
        }

        /* Get an argument and put it in numeral */
        v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
        if (d == 'D' && (v & 0x80000000)) {
            v = 0 - v;
            f |= 8;
        }
        i = 0;
        do {
            d = (char)(v % r); v /= r;
            if (d > 9) d += (c == 'x') ? 0x27 : 0x07;
            s[i++] = d + '0';
        } while (v && i < sizeof(s));
        if (f & 8) s[i++] = '-';
        j = i; d = (f & 1) ? '0' : ' ';
        while (!(f & 2) && j++ < w) wxputc(d);
        do wxputc(s[--i]); while (i);
        while (j++ < w) wxputc(' ');
    }
}

#if 0
void xprintf (          /* Put a formatted string to the default device */
    const char* fmt,    /* Pointer to the format string */
    ...                 /* Optional arguments */
)
{
    va_list arp;


    va_start(arp, fmt);
    xvprintf(fmt, arp);
    va_end(arp);
}
#endif

int _minicrt_sprintf(                 /* Put a formatted string to the memory */
                     char* buff,      /* Pointer to the output buffer */
                     const char* fmt, /* Pointer to the format string */
                     ...              /* Optional arguments */
)
{
    va_list arp;
    int len;

    va_start(arp, fmt);
    len = _minicrt_vsnprintf(buff, 0, fmt, arp);
    va_end(arp);
    return len;
}

int _minicrt_snprintf(                   /* Put a formatted string to the memory */
                      char* buff,        /* Pointer to the output buffer */
                      unsigned int size, /* Maximum size of buffer */
                      const char* fmt,   /* Pointer to the format string */
                      ...                /* Optional arguments */
)
{
    va_list arp;
    int len;

    va_start(arp, fmt);
    len = _minicrt_vsnprintf(buff, size, fmt, arp);
    va_end(arp);
    return len;
}

int _minicrt_vsnprintf(                   /* Put a formatted string to the memory */
                       char* buff,        /* Pointer to the output buffer */
                       unsigned int size, /* Maximum size of buffer */
                       const char* fmt,   /* Pointer to the format string */
                       va_list arp        /* Optional arguments */
)
{
    int len;

    outptr = buff;      /* Switch destination for memory */
    if (size > 0)
        outptrend = buff + size - 1;        // leave room for terminating null

    _minicrt_xvprintf(fmt, arp);

    *outptr = 0;        /* Terminate output string with a \0 */
    len = outptr - buff;
    outptr = 0;         /* Switch destination for device */
    outptrend = 0;
    return len;
}

int _minicrt_wsprintf(wchar_t* buff, const wchar_t* fmt, ...)
{
    va_list arp;
    int len;

    va_start(arp, fmt);
    len = _minicrt_wvsnprintf(buff, 0, fmt, arp);
    va_end(arp);
    return len;
}

int _minicrt_wsnprintf(wchar_t* buff, unsigned int size, const wchar_t* fmt, ...)
{
    va_list arp;
    int len;

    va_start(arp, fmt);
    len = _minicrt_wvsnprintf(buff, size, fmt, arp);
    va_end(arp);
    return len;
}

int _minicrt_wvsnprintf(wchar_t* buff, unsigned int size, const wchar_t* fmt, va_list arp)
{
    int len;

    woutptr = buff;
    if (size > 0)
        woutptrend = buff + size;

    _minicrt_wxvprintf(fmt, arp);

    *woutptr = 0;
    len = woutptr - buff;
    woutptr = 0;
    woutptrend = 0;
    return len;
}

#if 0
void xfprintf (                 /* Put a formatted string to the specified device */
    void(*func)(unsigned char), /* Pointer to the output function */
    const char* fmt,            /* Pointer to the format string */
    ...                         /* Optional arguments */
)
{
    va_list arp;
    void (*pf)(unsigned char);


    pf = xfunc_out;     /* Save current output device */
    xfunc_out = func;   /* Switch output to specified device */

    va_start(arp, fmt);
    xvprintf(fmt, arp);
    va_end(arp);

    xfunc_out = pf;     /* Restore output device */
}
#endif