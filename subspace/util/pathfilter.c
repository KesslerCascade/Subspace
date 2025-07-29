#include "pathfilter.h"
#include <cx/container/foreach.h>

void pathFilter(string* out, strref path)
{
    char* ob = strBuffer(out, strLen(path));
    uint32 ol = 0, p = 0;
    bool skipspc = true;

    foreach(string, si, path) {
        for (uint32 i = 0; i < si.len; i++) {
            uint8 c = si.bytes[i];
            if (skipspc && isspace(c))
                continue;   // trim whitespace at start

            if (isalnum(c) || c == ' ' || c == '_' || c == '-' || c == '(' || c == ')') {
                skipspc = false;
                ob[p++] = c;
                if (c != ' ')
                    ol = p;     // trim whitespace at end
            }
        }
    }

    strSetLen(out, ol);
}