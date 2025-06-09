#include <cx/string.h>
#include "run/runinfo.h"
#include "subspace.h"

void spointFormat(string* out, int64 spoint)
{
    strFromInt32(out, SPOINT_NUM(spoint), 10);
    int32 extra = SPOINT_EXTRA(spoint);

    if (extra > 0) {
        int len     = strLen(*out);
        char suffix = '\0';

        char* buf = strBuffer(out, len + 1);
        if (extra <= 26)
            suffix = 'a' + extra - 1;
        else if (extra <= 52)
            suffix = 'A' + extra - 27;
        // otherwise... uh I dunno

        buf[len] = suffix;
    }
}
