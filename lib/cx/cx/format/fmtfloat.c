#include "format_private.h"
#include "cx/utils/compare.h"

#define absv(n) ((n) < 0 ? -(n) : (n))

enum FloatOpts {
    FMT_FloatFixed = 0x00010000,
    FMT_FloatZero = 0x00020000,
    FMT_FloatDecDigits = 0x00040000,
};

_Use_decl_annotations_
bool _fmtParseFloatOpt(FMTVar *v, strref opt)
{
    int32 val;
    if (strEq(opt, _S"fixed")) {
        v->flags |= FMT_FloatFixed;
        return true;
    } else if (strEq(opt, _S"zero")) {
        v->flags |= FMT_FloatZero;
        return true;
    } else if (strBeginsWith(opt, _S"sig:")) {
        strSubStr(&v->tmp, opt, 4, strEnd);
        if (strToInt32(&val, v->tmp, 10, true) && val >= 1 && val < 18)
            v->fmtdata[0] = val;
        return true;
    } else if (strBeginsWith(opt, _S"dec:")) {
        strSubStr(&v->tmp, opt, 4, strEnd);
        if (strToInt32(&val, v->tmp, 10, true) && val >= 0) {
            v->fmtdata[1] = val;
            v->flags |= FMT_FloatDecDigits;
        }
        return true;
    }
    return false;
}

_Use_decl_annotations_
bool _fmtParseFloatFinalize(FMTVar *v)
{
    if (v->flags & FMT_FloatFixed && !(v->flags & FMT_FloatDecDigits)) {
        // fixed implies dec digits
        v->flags |= FMT_FloatDecDigits;
        // default to 6 digits in fixed mode
        v->fmtdata[1] = 6;
    }

    // we handle upper/lower (for the exponent) in the float formatter itself
    v->flags |= FMTVar_NoGenCase;

    // width will be padded in the float formatter
    v->flags |= FMTVar_NoGenWidth;

    return true;
}

// for special return values
static bool fmtFloatSpecial(_Inout_ FMTVar *v, _Inout_ string *out, _In_ strref str)
{
    v->flags &= (~FMTVar_NoGenCase | ~FMTVar_NoGenWidth);
    if (!(v->flags & (FMTVar_Left | FMTVar_Center)))
        v->flags |= FMTVar_Right;
    strDup(out, str);
    return true;
}

_Use_decl_annotations_
bool _fmtFloat(FMTVar *v, string *out)
{
    uint8 digits[18];
    int32 K, ndigits;
    bool neg;

    // handle all the special case stuff here
    switch (stGetId(v->type)) {
    case stTypeId(float32):
    {
        uint32 fpbits = *(uint32*)v->data;
        neg = fpbits & 0x80000000;
        if ((fpbits & 0x7f800000) == 0x7f800000) {
            if (fpbits & 0x007fffff)
                return fmtFloatSpecial(v, out, _S"NaN");
            else
                return fmtFloatSpecial(v, out, neg ? _S"-Inf" : _S"Inf");
        }

        if (fpbits & 0x7fffffff) {
            ndigits = _strnum_grisu2_32(*(float32*)v->data, digits, &K);
        } else {
            digits[0] = '0';
            ndigits = 1;
            K = 0;
        }
        break;
    }
    case stTypeId(float64):
    {
        uint64 fpbits = *(uint64*)v->data;
        neg = fpbits & 0x8000000000000000;
        if ((fpbits & 0x7ff0000000000000) == 0x7ff0000000000000) {
            if (fpbits & 0x000fffffffffffff)
                return fmtFloatSpecial(v, out, _S"NaN");
            else
                return fmtFloatSpecial(v, out, neg ? _S"-Inf" : _S"Inf");
            return true;
        }

        if (fpbits & 0x7fffffffffffffff) {
            ndigits = _strnum_grisu2_64(*(float64*)v->data, digits, &K);
        } else {
            digits[0] = '0';
            ndigits = 1;
            K = 0;
        }
        break;
    }
    default:
        return false;
    }

ndigits_changed:
    ;
    // first, calculate length
    int32 exp = absv(K + ndigits - 1);
    int32 offset = clamp(ndigits + K, 0, ndigits);  // which digit should have the decimal point
    int32 rounddigits = 0;                          // how many digits to remove and round
    int32 intlen = 0, fraclen = 0, fraczlen = 0, explen = 0;
    char sign = 0;
    bool usesci = false;

    if (!(v->flags & FMT_FloatFixed)) {
        // automatic mode
        if (K >= 0 && (exp < (ndigits + 7)))
            intlen = ndigits + K;       // plain integer
        else if (K < 0 && (K > -7 || exp < 4)) {
            // regular integer/fraction
            if (offset == 0)
                intlen = 1;     // 0.frac
            else
                intlen = offset;
            fraclen = -K;
        } else {
            // scientific notation
            usesci = true;
            intlen = offset = 1;
            fraclen = ndigits - 1;
            if (exp > 99)
                explen = 5;
            else if (exp > 9)
                explen = 4;
            else
                explen = 3;
        }
    } else {
        // fixed mode
        if (K >= 0)
            intlen = ndigits + K;
        else {
            if (offset == 0)
                intlen = 1;
            else
                intlen = offset;
            fraclen = -K;
        }
    }

    if (neg)
        sign = '-';         // TODO: localization
    else if (v->flags & FMTVar_SignAlways)
        sign = '+';
    else if (v->flags & FMTVar_SignPrefix)
        sign = ' ';

    if (v->fmtdata[0] > 0) {
        rounddigits = max(rounddigits, ndigits - (int32)v->fmtdata[0]);
    }

    if (v->flags & FMT_FloatDecDigits) {
        int32 declimit = (int32)v->fmtdata[1];
        // limit fraction digits
        if (usesci && fraclen > declimit)
            rounddigits = max(rounddigits, fraclen - declimit);
        else if (K < 0)
            rounddigits = max(rounddigits, clamphigh(-K - declimit, ndigits - offset));
        // zero-pad at the end
        if (v->flags & FMT_FloatZero)
            fraczlen = (int32)v->fmtdata[1] - fraclen;
    }

    int32 totallen = (sign ? 1 : 0) + intlen + (fraclen > 0 || fraczlen > 0 ? 1 : 0) + fraclen + fraczlen + explen;
    int32 width = clamplow(v->width, 0);

    if (width > 0 && totallen > width && !(v->flags & FMT_FloatZero)) {
        // reduce the fraction part to try to make it fit
        // not compatible with FloatZero which forces the digits to always be there
        rounddigits = max(rounddigits, clamphigh(totallen - width, ndigits - offset));
    }

    // do we need to round off any extra digits?
    if (rounddigits > 0) {
        int32 i;
        bool carry = false;
        ndigits -= rounddigits;
        K += rounddigits;

        if (ndigits == 0) {
            // rounding off everything is rare, but pathological dec:#
            // can do it on very small numbers
            if (K != 0)
                return fmtFloatSpecial(v, out, _S"0");
            // corner case alert: K == 0 means everything got rounded off, but there
            // was a digit in the tenths place which might actually round up to 1
            return fmtFloatSpecial(v, out, digits[0] < '5' ? _S"0" : _S"1");
        }

        for (i = ndigits - 1; i >= 0; --i) {
            if (!carry && digits[i + 1] < '5')
                break;          // rounds down, all done
            if (digits[i] < '9') {
                // rounds up, but no carry, easy enough
                digits[i]++;
                break;
            }

            digits[i] = '0';
            if (i == ndigits - 1 && ndigits > 1) {
                // don't leave trailing zeros
                ndigits--;
                K++;
            }
            carry = true;
        }

        if (i == -1) {
            // need an extra digit
            memmove(digits + 1, digits, ndigits);
            digits[0] = '1';
            ndigits++;
        }

        // recalculate for new digits
        // This may seem like a silly way to do it, but grisu2 is so fast that it's
        // actually more efficient to run this loop twice than to build the rounding
        // into the generation algorithm itself, and it beats snprintf even in the
        // worst case.
        goto ndigits_changed;
    }

    // okay, now construct the string
    strClear(out);
    uint8 *obuf = strBuffer(out, max(totallen, width));
    uint8 *start = obuf;

    int32 diff = width - totallen;
    if (totallen < width) {
        if (v->flags & FMTVar_LeadingZeros) {
            // do nothing here, fill in later
        } else if (v->flags & FMTVar_Left) {
            memset(obuf + totallen, ' ', diff);
        } else if (v->flags & FMTVar_Center) {
            size_t l1 = (width - totallen) / 2;
            memset(obuf, ' ', l1);
            start = obuf + l1;
            memset(start + totallen, ' ', diff - l1);
        } else {
            // FMTVar_Right
            memset(obuf, ' ', diff);
            start = obuf + diff;
        }
    }

    if (sign != 0)
        *start++ = sign;

    if (totallen < width && (v->flags & FMTVar_LeadingZeros)) {
        // finally do this after the sign/prefix
        memset(start, '0', diff);
        start += diff;
    }

    if (usesci || K >= 0 || offset > 0) {
        memcpy(start, digits, offset);
        start += offset;
        if (!usesci && K > 0) {
            memset(start, '0', K);
            start += K;
        }
    } else {
        *start++ = '0';         // special case
    }

    if (fraclen > 0 || fraczlen > 0) {
        *start++ = '.';         // TODO: localize!!!

        if (!usesci && ndigits + K < 0) {
            // leading zeroes due to exponent
            memset(start, '0', -(ndigits + K));
            start += -(ndigits + K);
        }
        memcpy(start, digits + offset, (intptr)ndigits - offset);
        start += (intptr)ndigits - offset;
        if (fraczlen > 0) {
            memset(start, '0', fraczlen);
            start += fraczlen;
        }
    }

    if (usesci) {
        *start++ = (v->flags & FMTVar_Upper) ? 'E' : 'e';
        *start++ = K + ndigits - 1 < 0 ? '-' : '+';

        int cent = 0;
        if (exp > 99) {
            cent = exp / 100;
            *start++ = cent + '0';
            exp -= cent * 100;
        }
        if (exp > 9) {
            int dec = exp / 10;
            *start++ = dec + '0';
            exp -= dec * 10;

        } else if (cent) {
            *start++ = '0';
        }

        *start++ = exp % 10 + '0';
    }

    if (width == 0)
        devAssert(start == obuf + totallen);
    else
        devAssert(start <= obuf + max(width, totallen));

    return true;
}
