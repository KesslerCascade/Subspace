#include "jsonparse.h"

#include <cx/string.h>
#include <cx/string/string_private_utf8.h>
#include <cx/format.h>
#include <cx/utils/compare.h>

#define MAX_PARSE_DEPTH 1000

typedef struct ParseState {
    int line;
    int depth;
    string errmsg;
    jsonParseCB callback;
    void *userdata;

    JSONParseEvent ev;
    JSONParseContext *ctx;
} ParseState;

static void jsonCtxDestroy(_Pre_valid_ _Post_invalid_ JSONParseContext *ctx)
{
    if (ctx->ctype == JSON_Object) {
        strDestroy(&ctx->cdata.curKey);
    }

    xaFree(ctx);
}

static void jsonClearEvent(_Inout_ JSONParseEvent *ev)
{
    if (ev->etype == JSON_Object_Key || ev->etype == JSON_String || ev->etype == JSON_Error) {
        strDestroy(&ev->edata.strData);
    }
    memset(&ev->edata, 0, sizeof(ev->edata));
    ev->etype = JSON_Parse_Unknown;
    ev->ctx = NULL;
}

static void jsonCallback(_Inout_ JSONParseEvent *ev, _In_ ParseState *ps)
{
    ev->ctx = ps->ctx;
    ps->callback(ev, ps->userdata);
}

#define SCRATCH_SZ 32
#define outAppend(ch) scratch[scratchp++] = ch; \
    if (scratchp == SCRATCH_SZ) { \
        strAppend(out, (string)scratch); \
        scratchp = 0; \
    }

static bool parseString(_Inout_ StreamBuffer *sb, _Inout_ string *out, _Inout_ ParseState *ps)
{
    bool ret = false;
    char scratch[SCRATCH_SZ+1] = { 0 };
    int scratchp = 0;

    bool escape = false;
    int useq = 0;               // digits reamining in unicode hex escape sequence
    int32 ucp;                  // unicode code point being built
    int32 lastucp = 0;          // for surrogate pairs
    unsigned char ch;

    strClear(out);

    size_t didread;
    while (sbufCRead(sb, &ch, 1, &didread)) {
        if (ch < 0x20) {
            strFormat(&ps->errmsg, _S"Invalid character code 0x${uint(hex)} in string", stvar(uint8, ch));
            break;
        }

        if (useq > 0) {
            // Unicode character hex escape sequence in progress
            int32 hexval = 0;
            uint8 utf[5];
            int utflen;

            useq--;
            if (ch >= '0' && ch <= '9')
                hexval = ch - '0';
            else if (ch >= 'a' && ch <= 'f')
                hexval = 10 + ch - 'a';
            else if (ch >= 'A' && ch <= 'F')
                hexval = 10 + ch - 'A';
            else {
                strFormat(&ps->errmsg, _S"Invalid hex digit '${int(utfchar)}", stvar(int32, ch));
                break;
            }

            ucp |= hexval << (useq * 4);

            if (useq == 0 && lastucp == 0) {
                if (ucp <= 0xd7ff || ucp >= 0xe000) {
                    // BMP
                    utflen = _strUTF8Encode(utf, ucp);
                    for (int i = 0; i < utflen; i++) {
                        outAppend(utf[i]);
                    }
                } else if (ucp >= 0xd800 && ucp <= 0xdbff) {
                    // first half of a surrogate pair
                    lastucp = (ucp & 0x3ff) << 10;
                    ucp = 0;
                } else {
                    // these are supposed to be for the second half!
                    strDup(&ps->errmsg, _S"Invalid first half of surrogate pair");
                    break;
                }
            } else if (useq == 0) {
                if (ucp >= 0xdc00 && ucp <= 0xdfff) {
                    // second half of a surrogate pair
                    ucp = lastucp | (ucp & 0x3ff) | 0x10000;
                    utflen = _strUTF8Encode(utf, ucp);
                    for (int i = 0; i < utflen; i++) {
                        outAppend(utf[i]);
                    }
                    lastucp = 0;
                } else {
                    strDup(&ps->errmsg, _S"Invalid second half of surrogate pair");
                    break;
                }
            }
        } else if (escape) {
            escape = false;

            if (lastucp && ch != 'u') {
                // first half of surrogate pair is REQUIRED to be immediately followed up with a second \uXXXX
                strDup(&ps->errmsg, _S"Surrogate pair missing second half");
                break;
            }

            switch (ch) {
            case '"':
                outAppend('"');
                break;
            case '\\':
                outAppend('\\');
                break;
            case '/':
                outAppend('/');
                break;
            case 'b':
                outAppend('\b');
                break;
            case 'f':
                outAppend('\f');
                break;
            case 'n':
                outAppend('\n');
                break;
            case 'r':
                outAppend('\r');
                break;
            case 't':
                outAppend('\t');
                break;
            case 'u':
                ucp = 0;
                useq = 4;
                break;
            default:
                strFormat(&ps->errmsg, _S"Invalid escape sequence '\\${int(utfchar)}", stvar(int32, ch));
                break;
            }
        } else if (ch == '"') {
            ret = true;
            // end of the string
            break;
        } else {
            if (ch == '\\') {
                escape = true;
            } else {
                if (lastucp) {
                    // first half of surrogate pair is REQUIRED to be immediately followed up with a second \uXXXX
                    strDup(&ps->errmsg, _S"Surrogate pair missing second half");
                    break;
                }

                outAppend(ch);
            }
        }
    }

    if (!ret && sbufCAvail(sb) == 0) {
        strDup(&ps->errmsg, _S"Unterminated string");
    }

    // copy any leftover output
    if (scratchp > 0) {
        scratch[scratchp] = '\0';
        strAppend(out, (string)scratch);
    }
    return ret;
}

_Success_(return)
static bool parseNumInt(_Inout_ StreamBuffer *sb, bool neg, size_t intoff, size_t intlen, _Out_ int64 *out, _Inout_ ParseState *ps)
{
    int64 res = 0;
    int64 mult = 1;
    unsigned char ch;

    if (intlen > 19)
        return false;       // can't possibly fit in an int64

    for (size_t i = 0; i < intlen; ++i) {
        // should be impossible to fail since we've already read it once
        sbufCPeek(sb, &ch, intoff + i, 1);
        ch -= '0';

        if (i == intlen - 1) {
            // overflow check
            if (!neg && (res > 922337203685477580 || (res == 922337203685477580 && ch > 7)))
                return false;
            if (neg && (res < -922337203685477580 || (res == -922337203685477580 && ch > 8)))
                return false;
        }

        res *= 10;
        if (!neg)
            res += ch * mult;
        else
            res -= ch * mult;
    }

    *out = res;
    return true;
}

_Success_(return)
static bool parseNumFloat(_Inout_ StreamBuffer *sb, bool neg, size_t intoff, size_t intlen, size_t fracoff, size_t fraclen, bool expneg, size_t expoff, size_t explen, _Out_ double *out, _Inout_ ParseState *ps)
{
    // TODO: convert to cx strToFloat if/when those are implemented.
    // For now use system strtod because these algorithms are very complex.
    size_t total = max(max(intoff + intlen, fracoff + fraclen), expoff + explen);
    char *buf = xaAlloc(total + 1);
    buf[total] = '\0';
    if (!sbufCPeek(sb, (uint8*)buf, 0, total))
        return false;

    *out = strtod(buf, NULL);
    xaFree(buf);
    return true;
}

static bool parseNumber(_Inout_ StreamBuffer *sb, _Inout_ JSONParseEvent *ev, _Inout_ ParseState *ps)
{
    unsigned char ch;
    int phase = 0;
    size_t off = 0;
    size_t intoff = 0;
    size_t intlen = 0;
    size_t fracoff = 0;
    size_t fraclen = 0;
    size_t expoff = 0;
    size_t explen = 0;
    bool neg = false, expneg = false;

    // phases:
    // 0 negative or initial digit
    // 1 integer digits
    // 2 fractional digits
    // 3 exponent sign or first digit
    // 4 exponent digits

    // first, split out the components into separate pieces
    while (sbufCFeed(sb, off + 1) && sbufCPeek(sb, &ch, off++, 1)) {
        if (phase > 0 && (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == ',')) {
            // all done
            // don't want to actually consume this character
            --off;
            break;
        }

        if (phase == 0 && ch == '-') {
            neg = true;
            phase = 1;
            intoff = 1;
        } else if ((phase == 0 || phase == 1) && ch >= '0' && ch <= '9') {
            intlen++;
            phase = 1;
        } else if (phase == 1 && ch == '.') {
            phase = 2;
        } else if (phase == 2 && ch >= '0' && ch <= '9') {
            if (fracoff == 0)
                fracoff = off - 1;
            fraclen++;
        } else if ((phase == 1 || phase == 2) && (ch == 'e' || ch == 'E')) {
            phase = 3;
        } else if (phase == 3 && ch == '+') {
            // positive exponent is the default, ignore
            phase = 4;
        } else if (phase == 3 && ch == '-') {
            expneg = true;
            phase = 4;
        } else if ((phase == 3 || phase == 4) && ch >= '0' && ch <= '9') {
            if (expoff == 0)
                expoff = off - 1;
            explen++;
            phase = 4;
        } else {
            strFormat(&ps->errmsg, _S"Found '${int(utfchar)}' where a number was expected", stvar(int32, ch));
            return false;
        }
    }

    // now actually parse the numbers
    // see if we can parse it as a simple integer
    if (fraclen == 0 && explen == 0 && parseNumInt(sb, neg, intoff, intlen, &ev->edata.intData, ps)) {
        ev->etype = JSON_Int;
        sbufCSkip(sb, off);
        return true;
    } else if (parseNumFloat(sb, neg, intoff, intlen, fracoff, fraclen, expneg, expoff, explen, &ev->edata.floatData, ps)) {
        ev->etype = JSON_Float;
        sbufCSkip(sb, off);
        return true;
    }

    return false;
}

static void skipWS(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps)
{
    unsigned char ch;

    while (sbufCFeed(sb, 1) && sbufCPeek(sb, &ch, 0, 1)) {
        switch (ch) {
        case '\n':
            ps->line++;
            // FALLTHROUGH
        case '\r':
        case '\t':
        case ' ':
            // whitespace, ignore
            sbufCSkip(sb, 1);
            break;
        default:
            return;
        }
    }
}

static void pushCtx(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps, int newctype)
{
    JSONParseContext *ctx = xaAlloc(sizeof(JSONParseContext), XA_Zero);
    ctx->ctype = newctype;
    ctx->parent = ps->ctx;
    ps->ctx = ctx;
    ps->depth++;
}

static bool popCtx(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps)
{
    if (!ps->ctx->parent)
        return false;

    JSONParseContext *ctx = ps->ctx;
    ps->ctx = ps->ctx->parent;
    ps->depth--;

    jsonCtxDestroy(ctx);

    return true;
}

static bool parseObject(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps);
static bool parseArray(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps);

static bool parseValue(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps)
{
    uint8 tmp[4];
    bool ret = false;
    unsigned char ch;

    jsonClearEvent(&ps->ev);
    skipWS(sb, ps);

    if (!(sbufCFeed(sb, 1) && sbufCPeek(sb, &ch, 0, 1)))
        return false;

    switch (ch) {
    case '{':
        sbufCSkip(sb, 1);
        ps->ev.etype = JSON_Object_Begin;
        jsonCallback(&ps->ev, ps);

        if (ps->depth >= MAX_PARSE_DEPTH) {
            strDup(&ps->errmsg, _S"Exceeded recursion limit");
            break;
        }

        pushCtx(sb, ps, JSON_Object);
        ret = parseObject(sb, ps);
        popCtx(sb, ps);

        break;
    case '[':
        sbufCSkip(sb, 1);
        ps->ev.etype = JSON_Array_Begin;
        jsonCallback(&ps->ev, ps);

        if (ps->depth >= MAX_PARSE_DEPTH) {
            strDup(&ps->errmsg, _S"Exceeded recursion limit");
            break;
        }

        pushCtx(sb, ps, JSON_Array);
        ret = parseArray(sb, ps);
        popCtx(sb, ps);

        break;
    case '"':
        sbufCSkip(sb, 1);

        // single value json (string)
        ret = parseString(sb, &ps->ev.edata.strData, ps);
        if (ret) {
            ps->ev.etype = JSON_String;
            jsonCallback(&ps->ev, ps);
        }
        break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
        ret = parseNumber(sb, &ps->ev, ps);
        if (ret)
            jsonCallback(&ps->ev, ps);
        break;
    case 't':
        if (!sbufCFeed(sb, 4) ||
            !sbufCPeek(sb, tmp, 1, 3) ||
            memcmp(tmp, "rue", 3) != 0)
            break;

        sbufCSkip(sb, 4);
        ps->ev.etype = JSON_True;
        jsonCallback(&ps->ev, ps);
        ret = true;
        break;
    case 'f':
        if (!sbufCFeed(sb, 5) ||
            !sbufCPeek(sb, tmp, 1, 4) ||
            memcmp(tmp, "alse", 4) != 0)
            break;

        sbufCSkip(sb, 5);
        ps->ev.etype = JSON_False;
        jsonCallback(&ps->ev, ps);
        ret = true;
        break;
    case 'n':
        if (!sbufCFeed(sb, 4) ||
            !sbufCPeek(sb, tmp, 1, 3) ||
            memcmp(tmp, "ull", 3) != 0)
            break;

        sbufCSkip(sb, 4);
        ps->ev.etype = JSON_Null;
        jsonCallback(&ps->ev, ps);
        ret = true;
        break;
    }

    if (ret)
        skipWS(sb, ps);

    return ret;
}

_Use_decl_annotations_
static bool parseObject(StreamBuffer *sb, ParseState *ps)
{
    bool ret = false;
    bool first = true;
    unsigned char ch;
    size_t didread;

    skipWS(sb, ps);

    while (sbufCFeed(sb, 1) && sbufCPeek(sb, &ch, 0, 1)) {
        if (ch == '}') {
            sbufCSkip(sb, 1);
            jsonClearEvent(&ps->ev);
            ps->ev.etype = JSON_Object_End;
            jsonCallback(&ps->ev, ps);
            ret = true;
            break;
        } else if (!first) {
            if (ch == ',') {
                // skip the comma
                sbufCSkip(sb, 1);
            } else {
                strFormat(&ps->errmsg, _S"Expected ',' after member \"${string}\" of object, got '${int(utfchar)}' instead",
                       stvar(strref, ps->ctx->cdata.curKey), stvar(int32, ch));
                break;
            }
        }

        // first get the key
        skipWS(sb, ps);
        if (!sbufCRead(sb, &ch, 1, &didread) || ch != '"') {
            strFormat(&ps->errmsg, _S"Expected '\"' to begin object key, got '${int(utfchar)}' instead",
                      stvar(int32, ch));
            break;
        }

        if (!parseString(sb, &ps->ctx->cdata.curKey, ps))
            break;
        jsonClearEvent(&ps->ev);
        ps->ev.etype = JSON_Object_Key;
        strDup(&ps->ev.edata.strData, ps->ctx->cdata.curKey);
        jsonCallback(&ps->ev, ps);
        skipWS(sb, ps);

        if (!sbufCRead(sb, &ch, 1, &didread) || ch != ':') {
            strFormat(&ps->errmsg, _S"Expected ':' in member \"${string}\" of object, got '${int(utfchar)}' instead",
                   stvar(strref, ps->ctx->cdata.curKey), stvar(int32, ch));
            break;
        }

        if (!parseValue(sb, ps))
            break;

        first = false;
    }

    return ret;
}

_Use_decl_annotations_
static bool parseArray(StreamBuffer *sb, ParseState *ps)
{
    bool ret = false;
    bool first = true;
    unsigned char ch;

    skipWS(sb, ps);

    while (sbufCFeed(sb, 1) && sbufCPeek(sb, &ch, 0, 1)) {
        if (ch == ']') {
            sbufCSkip(sb, 1);
            jsonClearEvent(&ps->ev);
            ps->ev.etype = JSON_Array_End;
            jsonCallback(&ps->ev, ps);
            ret = true;
            break;
        } else if (!first) {
            if (ch == ',') {
                // skip the comma
                sbufCSkip(sb, 1);
            } else {
                strFormat(&ps->errmsg, _S"Expected ',' after index ${int} of array, got '${int(utfchar)} instead",
                       stvar(int32, ps->ctx->cdata.curIdx - 1), stvar(int32, ch));
                break;
            }
        }

        if (!parseValue(sb, ps))
            break;

        first = false;
        ps->ctx->cdata.curIdx++;
    }

    if (!ret && sbufCAvail(sb) == 0) {
        strDup(&ps->errmsg, _S"Unterminated array");
    }

    return ret;
}


static bool parseTop(_Inout_ StreamBuffer *sb, _Inout_ ParseState *ps)
{
    ps->ctx->ctype = JSON_Top;
    // top-level context is really just a bare value
    return parseValue(sb, ps);
}

_Use_decl_annotations_
bool jsonParse(StreamBuffer *sb, jsonParseCB callback, void *userdata)
{
    bool ret = false;
    ParseState ps = { .line = 1 };

    if (!callback || !sbufCRegisterPull(sb, NULL, NULL))
        return false;

    ps.ctx = xaAlloc(sizeof(JSONParseContext), XA_Zero);

    ps.callback = callback;
    ps.userdata = userdata;

    ret = parseTop(sb, &ps);

    jsonClearEvent(&ps.ev);

    if (!ret) {
        ps.ev.etype = JSON_Error;
        ps.ev.ctx = ps.ctx;
        if (!strEmpty(ps.errmsg)) {
            strFormat(&ps.ev.edata.strData, _S"JSON Parse Error: ${string} on line ${int}",
                      stvar(strref, ps.errmsg),
                      stvar(int32, ps.line));
        } else {
            strFormat(&ps.ev.edata.strData, _S"JSON Parse Error: Invalid syntax on line ${int}", stvar(int32, ps.line));
        }
        ps.callback(&ps.ev, ps.userdata);
    }

    jsonClearEvent(&ps.ev);
    ps.ev.etype = JSON_End;
    ps.callback(&ps.ev, ps.userdata);

    jsonCtxDestroy(ps.ctx);
    strDestroy(&ps.errmsg);
    sbufCFinish(sb);
    return ret;
}
