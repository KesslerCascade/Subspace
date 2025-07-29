#include "pathsubst.h"
#include <cx/container.h>
#include <cx/format.h>

void pathSubst(string* out, strref path, hashtable varfmt)
{
    int vstate  = 0;   // var match state
    uint32 lpts = 0;   // last plain text start
    uint32 lpte = 0;   // last plain text end
    uint32 vns  = 0;   // var name start
    string temp = 0;

    strClear(out);
    strTemp(&temp, 32);

    foreach (string, si, path) {
        for (uint32 i = 0; i < si.len; i++) {
            uint8 c = si.bytes[i];

            if (vstate == 0 && c == '$') {
                vstate = 1;
            } else if (vstate == 1) {
                if (c == '{') {
                    if (lpte > lpts) {
                        strSubStr(&temp, path, lpts, lpte);
                        strAppend(out, temp);
                        lpts = lpte;
                    }

                    strClear(&temp);   // prepare to receive variable name
                    vstate = 2;
                } else {
                    // not actually a variable, add to plaintext
                    lpte   = i + 1;
                    vstate = 0;
                }
            } else if (vstate == 2) {
                if (c == '}') {
                    // check if there is a replacement for this variable and insert it if so
                    if (htFind(varfmt, string, temp, string, &temp)) {
                        strAppend(out, temp);
                    }
                    lpts   = i + 1;
                    vstate = 0;
                } else {
                    strSetChar(&temp, strEnd, c);
                }
            } else {
                // just a regular character
                lpte = i + 1;
            }
        }
    }

    // get any lingering bits at the end
    if (lpte > lpts) {
        strSubStr(&temp, path, lpts, lpte);
        strAppend(out, temp);
    }

    strDestroy(&temp);
}
