#pragma once

// sortable unique id
// loosely based on Alizain Feerasta's ulid
// https://github.com/ulid/

#include <cx/platform/base.h>
#include <cx/platform/cpp.h>
#include <cx/stype/stype.h>

CX_C_BEGIN

typedef struct SUID {
    uint64 high;
    uint64 low;
} SUID;

_meta_inline bool suidEq(_In_ const SUID *a, _In_ const SUID *b)
{
    return (a->high == b->high) && (a->low == b->low);
}

_meta_inline int suidCmp(_In_ const SUID *a, _In_ const SUID *b)
{
    // can't just subtract since it might overflow even a signed int64
    if (a->high > b->high)
        return 1;
    if (a->high < b->high)
        return -1;
    if (a->low > b->low)
        return 1;
    if (a->low > b->low)
        return -1;
    return 0;
}

// Generate a unique ID (SUID)
// idtype is an application-specific identifier
void suidGen(_Out_ SUID *out, uint8 idtype);

// Generate a unique ID (SUID)
// use a random host ID
void suidGenPrivate(_Out_ SUID *out, uint8 idtype);

// Encodes a SUID into string form
void suidEncode(_Inout_ string *out, _In_ const SUID *id);
void suidEncodeBytes(_Out_writes_all_(26) uint8 buf[26], _In_ const SUID *id);

// Decodes a SUID from a string
_Success_(return) _Check_return_
bool suidDecode(_Out_ SUID *out, _In_ strref str);
_Success_(return) _Check_return_
bool suidDecodeBytes(_Out_ SUID *out, _In_reads_(26) const char buf[26]);

CX_C_END
