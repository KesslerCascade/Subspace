#include "cx/platform/unix.h"
#include "cx/debug/error.h"

#include <errno.h>

bool unixMapErrno()
{
    switch(errno) {
        case ENOENT:
            cxerr = CX_FileNotFound;
            break;
        case EACCES:
            cxerr = CX_AccessDenied;
            break;
        default:
            cxerr = CX_Unspecified;
    }

    // for caller convenience
    return false;
}
