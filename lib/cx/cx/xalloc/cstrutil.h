#pragma once

#include <cx/platform/cpp.h>
#include <stddef.h>

CX_C_BEGIN

size_t cstrLen(_In_z_ const char *s);
_Ret_z_ char *cstrDup(_In_z_ const char *s);
size_t cstrLenw(_In_z_ const unsigned short *s);
_Ret_z_ unsigned short *cstrDupw(_In_z_ const unsigned short *s);
int cstrCmpi(_In_z_ const char *s1, _In_z_ const char *s2);

CX_C_END
