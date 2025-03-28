#pragma once

#include <cx/string/strbase.h>

CX_C_BEGIN

// Convert a string to a number
// strict indicates whether the entire string must be a number or if
// it may be followed by non-numeric characters
// NOTE: Unlike typical C library functions, a leading 0 does NOT imply base 8.
// Base 10 is the default unless otherwise specified, or the string begins with "0x".

_Success_(return) bool strToInt32(_Out_ int32 *_Nonnull out, _In_opt_ strref s, int base, bool strict);
_Success_(return) bool strToUInt32(_Out_ uint32 *_Nonnull out, _In_opt_ strref s, int base, bool strict);
_Success_(return) bool strToInt64(_Out_ int64 *_Nonnull out, _In_opt_ strref s, int base, bool strict);
_Success_(return) bool strToUInt64(_Out_ uint64 *_Nonnull out, _In_opt_ strref s, int base, bool strict);

bool strFromInt32(_Inout_ strhandle out, int32 i, uint16 base);
bool strFromUInt32(_Inout_ strhandle out, uint32 i, uint16 base);
bool strFromInt64(_Inout_ strhandle out, int64 i, uint16 base);
bool strFromUInt64(_Inout_ strhandle out, uint64 i, uint16 base);

_Success_(return) bool strToFloat32(_Out_ float32 *_Nonnull out, _In_opt_ strref s, bool strict);
_Success_(return) bool strToFloat64(_Out_ float64 *_Nonnull out, _In_opt_ strref s, bool strict);

bool strFromFloat32(_Inout_ strhandle out, float32 f);
bool strFromFloat64(_Inout_ strhandle out, float64 f);

// digit conversion internals, for format module or similar uses

// must contain enough characters to represent a 64-bit integer in base 2
// with preceding sign and null terminator
#define STRNUM_INTBUF 66
_Ret_valid_ uint8 *_Nonnull _strnum_u64toa(_Out_writes_(STRNUM_INTBUF) uint8 buf[STRNUM_INTBUF], _Out_opt_ uint32 *_Nullable len, uint64 val, uint16 base, uint32 mindigits, char sign, bool upper);

// 18 floating point digits is the maximum representable with 53 bit mantissa
#define STRNUM_FPDIGITS 18
// includes sign, scientific notation, and null terminator
#define STRNUM_FPBUF 25

// higher level interface -- gets actual number
uint32 _strnum_f64toa(float64 d, _Out_writes_(STRNUM_FPBUF) uint8 dest[STRNUM_FPBUF]);
uint32 _strnum_f32toa(float32 f, _Out_writes_(STRNUM_FPBUF) uint8 dest[STRNUM_FPBUF]);
// low level interface -- gets digits and exponent only
int32 _strnum_grisu2_32(float32 f, _Out_writes_(18) uint8* _Nonnull digits, _Inout_ int32* _Nonnull K);
int32 _strnum_grisu2_64(float64 d, _Out_writes_(18) uint8* _Nonnull digits, _Inout_ int32* _Nonnull K);

CX_C_END
