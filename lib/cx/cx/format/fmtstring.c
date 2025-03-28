#include "format_private.h"

enum StringOpts {
    FMT_StringEmpty     = 0x00010000,
    FMT_StringNull      = 0x00020000,
    FMT_StringNameCase  = 0x00040000,
};

_Use_decl_annotations_
bool _fmtParseStringOpt(FMTVar *v, strref opt)
{
    if (strEq(opt, _S"empty")) {
        v->flags |= FMT_StringEmpty;
        return true;
    } else if (strEq(opt, _S"null")) {
        v->flags |= FMT_StringNull;
        return true;
    } else if (strEq(opt, _S"name")) {
        v->flags |= FMT_StringNameCase;
        return true;
    }
    return false;
}

_Use_decl_annotations_
bool _fmtString(FMTVar *v, string *out)
{
    strref s = *(strref*)v->data;
    if ((v->flags & FMT_StringNull) && !s)
        return false;
    if ((v->flags & FMT_StringEmpty) && strEmpty(s))
        return false;

    strDup(out, s);

    if (v->flags & FMT_StringNameCase) {
        strLower(out);
        if (strLen(s) > 0) {
            uint8 ch = strGetChar(*out, 0);
            strSetChar(out, 0, toupper(ch));
        }
    }
    return true;
}
