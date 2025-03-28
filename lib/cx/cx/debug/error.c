#include "cx/debug/error.h"

_Thread_local int cxerr;

static const char *errormsgs[] = {
    "Winning!",
    "Unspecified Error",
    "Invalid Argument",
    "Access Denied",
    "File Not Found",
    "File Already Exists",
    "File is a Directory",
    "Read-only Filesystem",
};

_Use_decl_annotations_
const char *cxErrMsg(int err)
{
    if (err < 0 || err > sizeof(errormsgs) / sizeof(errormsgs[0]))
        return "Invalid Error Code";

    return errormsgs[err];
}
