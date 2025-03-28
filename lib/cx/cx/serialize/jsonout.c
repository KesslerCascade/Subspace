#include "jsonout.h"
#include <cx/cx.h>
#include <cx/string.h>
#include <cx/string/string_private_utf8.h>
#include <cx/container/foreach.h>

typedef struct JSONOut {
    StreamBuffer *sb;
    uint32 flags;

    string indent;
    int depth;

    bool first;
    bool isobjval;
    bool needeol;
} JSONOut;

static void writeStr(_Inout_ JSONOut *jo, _In_opt_ strref str)
{
    foreach(string, si, str)
    {
        sbufPWrite(jo->sb, si.bytes, si.len);
    }
}

static void writeStrEOL(_Inout_ JSONOut *jo, _In_opt_ strref str)
{
    writeStr(jo, str);

    if (jo->flags & JSON_Single_Line) {
        if (!(jo->flags & JSON_Compact))
            sbufPWrite(jo->sb, (uint8*)" ", 1);
        return;
    }

    if (jo->flags & JSON_Unix_EOL)
        sbufPWrite(jo->sb, (uint8*)"\n", 1);
    else if (jo->flags & JSON_Windows_EOL)
        sbufPWrite(jo->sb, (uint8*)"\r\n", 2);
}

_Use_decl_annotations_
JSONOut *jsonOutBegin(StreamBuffer *sb, flags_t flags)
{
    JSONOut *jo = xaAlloc(sizeof(JSONOut), XA_Zero);
    jo->sb = sb;
    jo->flags = flags;
    jo->first = true;

    if (!sbufPRegisterPush(sb, NULL, NULL)) {
        xaFree(jo);
        return NULL;
    }

    // set platform EOL value if not explicitly set
    if (!(flags & JSON_Unix_EOL) && !(flags & JSON_Windows_EOL)) {
#ifdef _PLATFORM_WIN
        jo->flags |= JSON_Windows_EOL;
#else
        jo->flags |= JSON_Unix_EOL;
#endif
    }

    // pre-compute indent spacing to use
    if (!(flags & JSON_Single_Line) && (flags & JSON_Indent_Mask) > 0) {
        uint8 *buf = strBuffer(&jo->indent, flags & JSON_Indent_Mask);
        memset(buf, ' ', flags & JSON_Indent_Mask);
    }

    return jo;
}

static void writeIndent(_Inout_ JSONOut *jo)
{
    if (!strEmpty(jo->indent)) {
        for (int i = 0; i < jo->depth; i++) {
            writeStr(jo, jo->indent);
        }
    }
}

static void beginVal(_Inout_ JSONOut *jo)
{
    // we're an object in a key/value pair, this has already been done for the key
    if (jo->isobjval) {
        jo->isobjval = false;
        return;
    }

    if (!jo->first)
        writeStrEOL(jo, _S",");
    else {
        if (jo->needeol)
            writeStrEOL(jo, _S"");
        jo->first = false;
        jo->needeol = false;
    }

    writeIndent(jo);
}

static bool writeIntVal(_Inout_ JSONOut *jo, int64 val)
{
    string temp = 0;
    if (!strFromInt64(&temp, val, 10))
        return false;

    writeStr(jo, temp);
    strDestroy(&temp);

    return true;
}

static bool writeFloatVal(_Inout_ JSONOut *jo, double val)
{
    string temp = 0;
    if (!strFromFloat64(&temp, val))
        return false;

    writeStr(jo, temp);
    strDestroy(&temp);

    return true;
}

static void utfEscapedEncode(_Inout_ string *out, int32 codepoint)
{
    string temp = 0;
    if (codepoint < 0x10000) {
        // can encode in a single sequence
        strFromUInt32(&temp, (uint32)codepoint, 16);
        strAppend(out, _S"\\u");
        for (int i = strLen(temp); i < 4; ++i) {
            strAppend(out, _S"0");
        }
        strAppend(out, temp);
    } else {
        // have to use surrogate pairs
        codepoint -= 0x10000;

        strFromUInt32(&temp, (uint32)(0xd800 | ((codepoint >> 10) & 0x3ff)), 16);
        strAppend(out, _S"\\u");
        for (int i = strLen(temp); i < 4; ++i) {
            strAppend(out, _S"0");
        }
        strAppend(out, temp);

        strFromUInt32(&temp, (uint32)(0xdc00 | (codepoint & 0x3ff)), 16);
        strAppend(out, _S"\\u");
        for (int i = strLen(temp); i < 4; ++i) {
            strAppend(out, _S"0");
        }
        strAppend(out, temp);
    }
    strDestroy(&temp);
}

static void writeEscapedString(_Inout_ JSONOut *jo, _In_opt_ strref val)
{
    string escaped = 0;
    uint8 buf[5];
    striter it;
    int32 code;

    strReset(&escaped, strLen(val) + (strLen(val) >> 1));

    striBorrow(&it, val);

    while (striU8Char(&it, &code)) {
        if (code < 0x20) {
            switch (code) {
            case '\b':
                strAppend(&escaped, _S"\\b");
                break;
            case '\f':
                strAppend(&escaped, _S"\\f");
                break;
            case '\n':
                strAppend(&escaped, _S"\\n");
                break;
            case '\r':
                strAppend(&escaped, _S"\\r");
                break;
            case '\t':
                strAppend(&escaped, _S"\\t");
                break;
            default:
                utfEscapedEncode(&escaped, code);
            }
        } else if (code == '"') {
            strAppend(&escaped, _S"\\\"");
        } else if (code == '\\') {
            strAppend(&escaped, _S"\\\\");
        } else if (code >= 0x20 && code <= 0x7f) {
            strSetChar(&escaped, strEnd, (char)code);
        } else {
            if (!(jo->flags & JSON_ASCII_Only)) {
                // insert raw UTF-8 into the string
                uint32 len = _strUTF8Encode(buf, code);
                buf[len] = '\0';
                strAppend(&escaped, (string)buf);
            } else {
                utfEscapedEncode(&escaped, code);
            }
        }
    }

    striFinish(&it);
    writeStr(jo, escaped);

    strDestroy(&escaped);
}

static bool writeStringVal(_Inout_ JSONOut *jo, _In_opt_ strref val)
{
    writeStr(jo, _S"\"");
    writeEscapedString(jo, val);
    writeStr(jo, _S"\"");

    return true;
}

_Use_decl_annotations_
bool jsonOut(JSONOut *jo, JSONParseEvent *ev)
{
    bool ret = true;

    switch (ev->etype) {
    case JSON_Object_Begin:
        beginVal(jo);
        writeStr(jo, _S"{");
        jo->first = true;
        jo->isobjval = false;
        jo->needeol = true;
        jo->depth++;
        break;
    case JSON_Object_Key:
        beginVal(jo);
        jo->isobjval = true;
        writeStr(jo, _S"\"");
        writeEscapedString(jo, ev->edata.strData);
        if (jo->flags & JSON_Compact)
            writeStr(jo, _S"\":");
        else
            writeStr(jo, _S"\": ");
        break;
    case JSON_Object_End:
        if (jo->depth == 0)
            return false;

        jo->depth--;
        if (!jo->first) {
            writeStrEOL(jo, _S"");
            writeIndent(jo);
        } else if (!(jo->flags & JSON_Compact)) {
            writeStr(jo, _S" ");
        }

        jo->first = false;
        jo->isobjval = false;
        jo->needeol = false;
        writeStr(jo, _S"}");
        break;
    case JSON_Array_Begin:
        beginVal(jo);
        writeStr(jo, _S"[");
        jo->first = true;
        jo->isobjval = false;
        jo->needeol = true;
        jo->depth++;
        break;
    case JSON_Array_End:
        if (jo->depth == 0)
            return false;

        jo->depth--;
        if (!jo->first) {
            writeStrEOL(jo, _S"");
            writeIndent(jo);
        } else if (!(jo->flags & JSON_Compact)) {
            writeStr(jo, _S" ");
        }

        jo->first = false;
        jo->isobjval = false;
        jo->needeol = false;
        writeStr(jo, _S"]");
        break;
    case JSON_Int:
        beginVal(jo);
        ret = writeIntVal(jo, ev->edata.intData);
        break;
    case JSON_Float:
        beginVal(jo);
        ret = writeFloatVal(jo, ev->edata.floatData);
        break;
    case JSON_String:
        beginVal(jo);
        ret = writeStringVal(jo, ev->edata.strData);
        break;
    case JSON_True:
        beginVal(jo);
        writeStr(jo, _S"true");
        break;
    case JSON_False:
        beginVal(jo);
        writeStr(jo, _S"false");
        break;
    case JSON_Null:
        beginVal(jo);
        writeStr(jo, _S"null");
        break;
    case JSON_Error:
    case JSON_End:
        break;
    default:
        ret = false;
    }

    return ret;
}

_Use_decl_annotations_
void jsonOutEnd(JSONOut **jo)
{
    if (!((*jo)->flags & JSON_Single_Line))
        writeStrEOL((*jo), _S"");
    sbufPFinish((*jo)->sb);
    strDestroy(&(*jo)->indent);

    xaFree(*jo);
}
