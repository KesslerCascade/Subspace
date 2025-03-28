#include "format_private.h"
#include "cx/string/string_private.h"

// must contain enough characters to represent a 64-bit integer in base 16
// with 0x prefix and null terminator
#define BUFSZ 19

enum PtrOpts {
    FMT_PtrPrefix = 0x00010000,
};

_Use_decl_annotations_
bool _fmtParsePtrOpt(FMTVar *v, strref opt)
{
    if (strEq(opt, _S"prefix")) {
        v->flags |= FMT_PtrPrefix;
        return true;
    }
    return false;
}

_Use_decl_annotations_
bool _fmtParsePtrFinalize(FMTVar *v)
{
    // we handle upper/lower in the ptr formatter itself to exclude the prefix
    v->flags |= FMTVar_NoGenCase;

    return true;
}

_Use_decl_annotations_
bool _fmtPtr(FMTVar *v, string *out)
{
    uint8 buf[BUFSZ];
    uint8 *p = buf + BUFSZ;
    char *cset;
    uint64 val = 0;
    uint32 val32;

    cset = (v->flags & FMTVar_Upper) ? _strnum_udigits : _strnum_ldigits;

    val = *(uintptr*)v->data;

    *--p = 0;       // null terminator

    while (val > 0xffffffff) {
        *--p = cset[val & 0xf];
        val >>= 4;
    }
    val32 = (uint32)val;
    do {
        *--p = cset[val32 & 0xf];
        val32 >>= 4;
    } while (val32 > 0);

    size_t buflen = buf + BUFSZ - p - 1;

    // leading zeroes apply *before* prefix is added
    if ((v->flags & FMTVar_LeadingZeros) && buflen < sizeof(uintptr_t) * 2) {
        for (size_t i = 0; i < sizeof(uintptr_t) * 2 - buflen; i++)
            *--p = '0';
    }

    if (v->flags & FMT_PtrPrefix) {
        *--p = 'x';
        *--p = '0';
    }

    strCopy(out, (string)p);

    return true;
}
