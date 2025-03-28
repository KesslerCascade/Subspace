#pragma once

#include "cx/string.h"
#include "cx/utils/compare.h"
#include "cx/thread/atomic.h"

typedef struct str_ref* _Nonnull string_v;                  // validated string
typedef const struct str_ref* _Nonnull strref_v;            // validated strref
typedef string_v* _Nonnull strhandle_v;                     // handle to validated string

// Flags field

#define STR_LEN0     0x00       // no length field, 8-bit ref count if STR_ALLOC
#define STR_LEN8     0x01       // 8-bit length, 8-bit ref count if STR_ALLOC
#define STR_LEN16    0x02       // 16-bit length, 16-bit ref count if STR_ALLOC
#define STR_LEN32    0x03       // 32-bit length, 16-bit ref count if STR_ALLOC
#define STR_LEN_MASK 0x03

#define STR_STACK    0x04       // string is allocated on the stack, ref count is buffer size
#define STR_ROPE     0x08       // string is a rope node
#define STR_ALLOC    0x10       // string allocated by us, ref count field present
#define STR_UTF8     0x20       // string is UTF-8
#define STR_ASCII    0x40       // string is 7-bit ASCII
#define STR_CX       0x80       // string contains cx header, otherwise plain C string

#define STR_ENCODING_MASK (STR_UTF8 | STR_ASCII)

#define STR_HDRP(s) ((uint8*)(s))
#define STR_HDR(s) (((*STR_HDRP(s) & STR_CX) && STR_HDRP(s)[1] == 0xc1) ? *STR_HDRP(s) : 0)
#define STR_FIELD(s, off, typ) (*(typ*)(&STR_HDRP(s)[off]))

#define STR_OT_LEN 0
#define STR_OT_REF 1
#define STR_OT_STR 2
extern const uint8 _str_off[32];
// dirty trick here, since STR_ALLOC is defined as 16, it can be used
// as-is as a base index into the table without further masking or shifting
#define STR_OFF_BASE(hdr) ((hdr & STR_ALLOC) | (hdr & STR_LEN_MASK) << 2)
#define STR_OFF_LEN(hdr) (_str_off[STR_OFF_BASE(hdr)])
#define STR_OFF_REF(hdr) (_str_off[STR_OFF_BASE(hdr) | STR_OT_REF])
#define STR_OFF_STR(hdr) (hdr & STR_CX ? (_str_off[STR_OFF_BASE(hdr) | STR_OT_STR]) : 0)

#define STR_LEN8_LEN(s) STR_FIELD(s, _strOffLen(_strHdr(s)), uint8)
#define STR_LEN16_LEN(s) STR_FIELD(s, _strOffLen(_strHdr(s)), uint16)
#define STR_LEN32_LEN(s) STR_FIELD(s, _strOffLen(_strHdr(s)), uint32)

#define STR_LEN8_REF(s) STR_FIELD(s, _strOffRef(_strHdr(s)), uint8)
#define STR_LEN16_REF(s) STR_FIELD(s, _strOffRef(_strHdr(s)), uint16)

#define STR_BUFFER(s) (&STR_FIELD(s, _strOffStr(_strHdr(s)), uint8))
#define STR_ROPEDATA(s) (&STR_FIELD(s, _strOffStr(_strHdr(s)), str_ropedata))

_meta_inline uint8 _strOffLen(uint8 hdr)
{
    return STR_OFF_LEN(hdr);
}

_meta_inline uint8 _strOffRef(uint8 hdr)
{
    return STR_OFF_REF(hdr);
}

_meta_inline uint8 _strOffStr(uint8 hdr)
{
    return STR_OFF_STR(hdr);
}

_Ret_valid_
_meta_inline uint8 * _Nonnull _strHdrP(_In_ strref_v s)
{
    return STR_HDRP(s);
}

_meta_inline uint8 _strHdr(_In_ strref_v s)
{
    return STR_HDR(s);
}

_Ret_valid_
_meta_inline uint8 * _Nonnull _strBuffer(_In_ strref_v s)
{
    return STR_BUFFER(s);
}

#define STR_CHECK_VALID(s) (s && *(uint8*)s)
#define STR_SAFE_DEREF(ps) ((ps && *ps) ? *ps : 0)

_meta_inline uint32 _strFastLen(_In_ strref_v s)
{
    if (!(STR_HDR(s) & STR_CX))
        return (uint32)cstrLen((const char*)s);
    switch (STR_HDR(s) & STR_LEN_MASK) {
    case STR_LEN8:
        return STR_LEN8_LEN(s);
    case STR_LEN16:
        return STR_LEN16_LEN(s);
    case STR_LEN32:
        return STR_LEN32_LEN(s);
    default: // STR_LEN0
        return (uint32)cstrLen((const char*)_strBuffer(s));
    }
}

// must only be used on STR_CX | STR_ALLOC strings!
_meta_inline uint16 _strFastRef(_In_ strref_v s)
{
    int l = STR_HDR(s) & STR_LEN_MASK;

    if (l <= STR_LEN8)
        return atomicLoad(uint8, &STR_FIELD(s, STR_OFF_REF(STR_HDR(s)), atomic(uint8)), Acquire);
    else
        return atomicLoad(uint16, &STR_FIELD(s, STR_OFF_REF(STR_HDR(s)), atomic(uint16)), Acquire);
}

_meta_inline uint16 _strFastRefNoSync(_In_ strref_v s)
{
    int l = STR_HDR(s) & STR_LEN_MASK;

    if (l <= STR_LEN8)
        return STR_LEN8_REF(s);
    else
        return STR_LEN16_REF(s);
}

// Always round up allocations to avoid constant realloc
#define STR_ALLOC_SIZE 16

// rope tuning
// try not to create a rope smaller than this
#define ROPE_MIN_SIZE 64
// use a rope if final join size exceeds this
#define ROPE_JOIN_THRESH 128
// use a rope if substring size exceeds this
#define ROPE_SUBSTR_THRESH 96
// maximum size to merge together on joins
#define ROPE_MAX_MERGE 256

#include "string_private_utf8.h"

// dummy empty string used as input when NULL or invalid strings are passed in
extern string_v _strEmpty;

// resets the string internals, re-use memory if possible
void _strReset(_Inout_ptr_opt_ strhandle s, uint32 minsz);

// these change the structure internally and can result in inconsistent state
// if not used with care
void _strSetLen(_Inout_ string_v s, uint32 len);
void _strInitRef(_Inout_ string_v s);
void _strSetRef(_Inout_ string_v s, uint16 ref);

// ensure that ps is allocated by us and has a single reference
void _strMakeUnique(_Inout_ptr_ strhandle_v ps, uint32 minszforcopy);
// like _strMakeUnique but also flattens ropes into plain strings
void _strFlatten(_Inout_ptr_ strhandle_v ps, uint32 minszforcopy);
// resize ps in place if possible, or copy if necessary (changing ps).
// resizes buffer only, does NOT zero buffer or set length header
void _strResize(_Inout_ptr_ strhandle_v ps, uint32 len, bool unique);
// duplicates s and returns a copy, optionally with more reserved space allocated
_Ret_valid_ string_v _strCopy(_In_ strref_v s, uint32 minsz);
// direct copy of string buffer or rope internals, does not check destination size!
uint32 _strFastCopy(_In_ strref_v s, uint32 off, _Out_writes_bytes_(bytes) uint8 *_Nonnull buf, uint32 bytes);

// faster concatenation for internal use
bool _strConcatNoRope(_Inout_ strhandle o, _In_opt_ strref s1, _In_opt_ strref s2);

// rope data comes directly after string header if STR_ROPE is set
typedef struct str_roperef {
    string str;
    uint32 off;
    uint32 len;
} str_roperef;
typedef struct str_ropedata {
    str_roperef left;
    str_roperef right;
    int depth;
} str_ropedata;

_Ret_valid_
_meta_inline str_ropedata *_strRopeData(_In_ strref_v s)
{
    return STR_ROPEDATA(s);
}

string_v _strCreateRope(_In_ strref_v left, uint32 left_off, uint32 left_len, _In_opt_ strref right, uint32 right_off, uint32 right_len, bool balance);
string_v _strCreateRope1(_In_ strref_v s, uint32 off, uint32 len);
string_v _strCloneRope(_In_ strref_v s);
void _strDestroyRope(_Inout_ string_v s);
uint32 _strRopeFastCopy(_In_ strref_v s, uint32 off, _Out_writes_bytes_(bytes) uint8 *_Nonnull buf, uint32 bytes);
_Success_(return) bool _strRopeRealStr(_Inout_ strhandle_v s, uint32 off, _Out_ strhandle_v rs, _Out_ uint32 *_Nonnull rsoff, _Out_ uint32 *_Nonnull rslen, _Out_ uint32 *_Nonnull rsstart, bool writable);

// Finds first occurrence of find in s at or after start
int32 _strFindChar(_In_ strref_v s, int32 start, char find);
// Finds last occurrence of find in s before end
// end can be 0 to indicate end of the string
int32 _strFindCharR(_In_ strref_v s, int32 end, char find);

_meta_inline uint8 _strFastChar(_In_ strref s, uint32 i)
{
    if (!(STR_HDR(s) & STR_ROPE)) {
        return _strBuffer(s)[i];
    } else {
        string realstr;
        uint32 realoff, reallen, realstart;
        if (_strRopeRealStr((string*)&s, i, &realstr, &realoff, &reallen, &realstart, false))
            return _strBuffer(realstr)[realoff];
    }
    return 0;
}

extern char _strnum_udigits[];
extern char _strnum_ldigits[];
