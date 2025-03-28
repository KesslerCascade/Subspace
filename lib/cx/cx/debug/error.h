#pragma once

#include <cx/cx.h>

CX_C_BEGIN

extern _Thread_local int cxerr;

enum CX_ERROR {
    CX_Success = 0,
    CX_Unspecified,             // catch-all for unknown errors
    CX_InvalidArgument,         // your argument is invalid - you lost!
    CX_AccessDenied,            // can't touch this
    CX_FileNotFound,            // new OS who dis?
    CX_AlreadyExists,           // cowardly refusing to overwrite
    CX_IsDirectory,             // tried to treat a directory like a file
    CX_ReadOnly,                // tried to write to a read-only vfs path
    CX_Range,                   // Value out of range
};

_Ret_z_
const char *cxErrMsg(int err);

CX_C_END
