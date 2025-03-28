#include "string_private.h"
#include "cx/container/sarray.h"

_Use_decl_annotations_
int32 strSplit(sa_string *_Nonnull out, strref s, strref sep, bool empty)
{
    saClear(out);

    uint32 seplen = strLen(sep);
    int32 start = 0, next;
    string seg = 0;
    while ((next = strFind(s, start, sep)) != -1) {
        if (start != next || empty) {
            if (next != 0)      // degenerate case for starting with separator
                strSubStr(&seg, s, start, next);
            else
                strClear(&seg);
            saPushC(out, string, &seg);
        }
        start = next + seplen;
    }
    if (start < (int32)strLen(s) || empty) {
        strSubStr(&seg, s, start, strLen(s));
        saPushC(out, string, &seg);
    }

    return saSize(*out);
}
