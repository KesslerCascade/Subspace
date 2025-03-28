#pragma once

#include <cx/string/strbase.h>

CX_C_BEGIN

// Compare two strings
// Slightly faster than strCmp you only care about equality.
// Returns: true if the strings are equal, otherwise false
_Pure bool strEq(_In_opt_ strref s1, _In_opt_ strref s2);
_Pure bool strEqi(_In_opt_ strref s1, _In_opt_ strref s2);

// Compare two strings
// This is purely a binary comparison and is not intended for lexical sorting.
// Returns: < 0 if s1 comes before s2
//            0 if the strings are equal
//          > 0 if s1 comes after s2
_Pure int32 strCmp(_In_opt_ strref s1, _In_opt_ strref s2);
_Pure int32 strCmpi(_In_opt_ strref s1, _In_opt_ strref s2);

_Pure bool strRangeEq(_In_opt_ strref str, _In_opt_ strref sub, int32 off, uint32 len);
_Pure bool strRangeEqi(_In_opt_ strref str, _In_opt_ strref sub, int32 off, uint32 len);
_Pure int32 strRangeCmp(_In_opt_ strref str, _In_opt_ strref sub, int32 off, uint32 len);
_Pure int32 strRangeCmpi(_In_opt_ strref str, _In_opt_ strref sub, int32 off, uint32 len);

_Pure bool strBeginsWith(_In_opt_ strref str, _In_opt_ strref sub);
_Pure bool strBeginsWithi(_In_opt_ strref str, _In_opt_ strref sub);
_Pure bool strEndsWith(_In_opt_ strref str, _In_opt_ strref sub);
_Pure bool strEndsWithi(_In_opt_ strref str, _In_opt_ strref sub);

CX_C_END
