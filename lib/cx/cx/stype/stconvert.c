#include "cx/cx.h"

#include "cx/obj/stype_obj.h"
#include "cx/container/stype_sarray.h"
#include "cx/string/stype_string.h"
#include "cx/container/stype_hashtable.h"
#include "cx/stype/stype_stvar.h"
#include "cx/suid/stype_suid.h"
#include "cx/string.h"

#define stConvertNoneZero(type) case stTypeId(type): \
    dest->st_##type = 0;                             \
    return true
_Success_(return) _Check_return_
bool stConvert_none(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    switch (stGetId(destst)) {
    case stTypeId(bool):
        dest->st_bool = false;
        return true;
    case stTypeId(stvar):
        dest->st_stvar->type = stType(none);
        return true;
    case stTypeId(string):
        dest->st_string = NULL;
        return true;
    stConvertNoneZero(int8);
    stConvertNoneZero(int16);
    stConvertNoneZero(int32);
    stConvertNoneZero(int64);
    stConvertNoneZero(uint8);
    stConvertNoneZero(uint16);
    stConvertNoneZero(uint32);
    stConvertNoneZero(uint64);
    stConvertNoneZero(float32);
    stConvertNoneZero(float64);
    stConvertNoneZero(ptr);
    }
    return false;
}

#define stConvertBoolNum(type) case stTypeId(type):     \
    dest->st_##type = src.st_bool ?                     \
        (stTypeDef(type))(1) : (stTypeDef(type))(0);    \
    return true
_Success_(return) _Check_return_
bool stConvert_bool(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    switch (stGetId(destst)) {
    case stTypeId(bool):
        dest->st_bool = src.st_bool;
        return true;
    stConvertBoolNum(int8);
    stConvertBoolNum(int16);
    stConvertBoolNum(int32);
    stConvertBoolNum(int64);
    stConvertBoolNum(uint8);
    stConvertBoolNum(uint16);
    stConvertBoolNum(uint32);
    stConvertBoolNum(uint64);
    stConvertBoolNum(float32);
    stConvertBoolNum(float64);
    case stTypeId(string):
        dest->st_string = 0;
        strDup(&dest->st_string, src.st_bool ? (string)"\xE1\xC1\x04""True" : (string)"\xE1\xC1\x05""False");
        return true;
    case stTypeId(stvar):
        dest->st_stvar->type = srcst;
        dest->st_stvar->data = src;
        return true;
    }

    return false;
}

#define stConvertSIntInput(type) case stTypeId(type): \
    v.s = (int64)src.st_##type;                       \
    break
#define stConvertUIntInput(type) case stTypeId(type): \
    v.u = (uint64)src.st_##type;                      \
    break

#define stConvertSIntOutput(type, vmin, vmax) case stTypeId(type):               \
    if (!norange && (v.s < vmin || (srcunsigned ? (v.u > vmax) : (v.s > vmax)))) \
        return false;                                                            \
    dest->st_##type = (stTypeDef(type))v.s;                                      \
    return true 

#define stConvertUIntOutput(type, vmax) case stTypeId(type):            \
    if (!norange && v.u > vmax)                                         \
        return false;                                                   \
    dest->st_##type = (stTypeDef(type))v.u;                             \
    return true 

_Success_(return) _Check_return_
bool stConvert_int(stype destst, _stCopyDest_Anno_(destst) stgeneric * dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    bool norange = flags & ST_Overflow;
    bool lossless = flags & ST_Lossless;
    bool srcunsigned = (STYPE_CLASS(srcst) == STCLASS_UINT);

    // to keep this simpler and avoid needing N*M combinations, internally convert to a 64-bit integer first
    union {
        int64 s;
        uint64 u;
    } v;

    switch (stGetId(srcst)) {
    stConvertSIntInput(int8);
    stConvertSIntInput(int16);
    stConvertSIntInput(int32);
    stConvertSIntInput(int64);
    stConvertUIntInput(uint8);
    stConvertUIntInput(uint16);
    stConvertUIntInput(uint32);
    stConvertUIntInput(uint64);
    stConvertUIntInput(ptr);
    default:
        // should be impossible to get here???
        return false;
    }

    // if we're converting signed to unsigned, check for the obvious negative number
    if (!norange &&
        !srcunsigned &&
        STYPE_CLASS(destst) == STCLASS_UINT &&
        v.s < 0)
        return false;

    switch (stGetId(destst)) {
    stConvertSIntOutput(int8, MIN_INT8, MAX_INT8);
    stConvertSIntOutput(int16, MIN_INT16, MAX_INT16);
    stConvertSIntOutput(int32, MIN_INT32, MAX_INT32);
    stConvertUIntOutput(uint8, MAX_UINT8);
    stConvertUIntOutput(uint16, MAX_UINT16);
    stConvertUIntOutput(uint32, MAX_UINT32);
    stConvertUIntOutput(ptr, MAX_UINTPTR);
    case stTypeId(int64):
        dest->st_int64 = v.s;
        return true;
    case stTypeId(uint64):
        dest->st_uint64 = v.u;
        return true;
    case stTypeId(bool):
        dest->st_bool = (v.u != 0);
        return true;
    case stTypeId(string):
        dest->st_string = 0;
        if (srcunsigned)
            return strFromUInt64(&dest->st_string, v.u, 10);
        return strFromInt64(&dest->st_string, v.s, 10);
    case stTypeId(float32):
        // check limits of what can be losslessly represented as a float
        if (lossless &&
            ((!srcunsigned && v.s < -16777216) ||
             v.u > 16777216))
            return false;
        dest->st_float32 = srcunsigned ? (float32)v.u : (float32)v.s;
        return true;
    case stTypeId(float64):
        // check limits of what can be losslessly represented as a float
        if (lossless &&
            ((!srcunsigned && v.s < -9007199254740992LL) ||
             v.u > 9007199254740992ULL))
            return false;
        dest->st_float64 = srcunsigned ? (float64)v.u : (float64)v.s;
        return true;
    case stTypeId(stvar):
        // okay, sure, we can put it in one
        dest->st_stvar->type = srcst;
        dest->st_stvar->data = src;
        return true;
    }

    // not a type we know how to convert to
    return false;
}

_Success_(return) _Check_return_
bool stConvert_float32(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    bool norange = flags & ST_Overflow;
    float32 val = src.st_float32;

    if (STYPE_CLASS(destst) == STCLASS_INT) {
        // int64 checks happen here; if it fits into an int64 we let the int conversion code handle smaller sizes
        if (!norange && (val < (float64)MIN_INT64 || val > (float64)MAX_INT64))
            return false;
        return stConvert_int(destst, dest, stType(int64), stgeneric(int64, (int64)val), flags);
    } else if (STYPE_CLASS(destst) == STCLASS_UINT) {
        if (!norange && (val < 0 || val > (float64)MAX_UINT64))
            return false;
        return stConvert_int(destst, dest, stType(uint64), stgeneric(uint64, (uint64)val), flags);
    }

    switch (stGetId(destst)) {
    case stTypeId(bool):
        dest->st_bool = (val != 0);
        return true;
    case stTypeId(string):
        dest->st_string = 0;
        return strFromFloat32(&dest->st_string, val);
    case stTypeId(float32):
        dest->st_float32 = val;
        return true;
    case stTypeId(float64):
        dest->st_float64 = val;
        return true;
    case stTypeId(stvar):
        dest->st_stvar->type = srcst;
        dest->st_stvar->data = src;
        return true;
    }

    return false;
}

_Success_(return) _Check_return_
bool stConvert_float64(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    bool norange = flags & ST_Overflow;
    bool lossless = flags & ST_Lossless;
    float64 val = src.st_float64;

    if (STYPE_CLASS(destst) == STCLASS_INT) {
        // int64 checks happen here; if it fits into an int64 we let the int conversion code handle smaller sizes
        if (!norange && (val < (float64)MIN_INT64 || val > (float64)MAX_INT64))
            return false;
        return stConvert_int(destst, dest, stType(int64), stgeneric(int64, (int64)val), flags);
    } else if (STYPE_CLASS(destst) == STCLASS_UINT) {
        if (!norange && (val < 0 || val > (float64)MAX_UINT64))
            return false;
        return stConvert_int(destst, dest, stType(uint64), stgeneric(uint64, (uint64)val), flags);
    }

    switch (stGetId(destst)) {
    case stTypeId(bool):
        dest->st_bool = (val != 0);
        return true;
    case stTypeId(string):
        dest->st_string = 0;
        return strFromFloat64(&dest->st_string, val);
    case stTypeId(float32):
        // Can never REALLY downconvert floating point types without losing some precision.
        // Arguably integers, MAYBE? Even then who's to say if it's supposed to be 5 or 5.0000000001
        if (lossless)
            return false;
        dest->st_float32 = (float32)val;
        return true;
    case stTypeId(float64):
        dest->st_float64 = val;
        return true;
    case stTypeId(stvar):
        dest->st_stvar->type = srcst;
        dest->st_stvar->data = src;
        return true;
    }

    return false;
}

_Success_(return) _Check_return_
bool stConvert_ptr(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, uint32 flags)
{
    // special handling for string
    if (stGetId(destst) == stTypeId(string)) {
        dest->st_string = 0;
        string temp = 0;
#ifdef _32BIT
        strTemp(&temp, 8);
        if (!strFromUInt32(&temp, (uint32)src.st_ptr, 16))
            return false;
#else
        strTemp(&temp, 16);
        if (!strFromUInt64(&temp, (uint64)src.st_ptr, 16))
            return false;
#endif
        strConcat(&dest->st_string, _S"0x", temp);
        return true;
    }

    // otherwise treat it like an unsigned integer
    return stConvert_int(destst, dest, srcst, src, flags);
}

alignMem(64) stConvertFunc _stDefaultConvert[256] = {
    // STCLASS_OPAQUE
    stConvert_none, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, stConvert_int, stConvert_int, 0, stConvert_int, 0, 0, 0, stConvert_int, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_UINT
    0, stConvert_int, stConvert_int, stConvert_bool, stConvert_int, 0, 0, 0, stConvert_int, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_FLOAT
    0, 0, 0, 0, stConvert_float32, 0, 0, 0, stConvert_float64, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_PTR
    0, 0, 0, 0, sizeof(void *) == sizeof(int32) ? stConvert_ptr : 0, 0, 0, 0,
    sizeof(void *) == sizeof(int64) ? stConvert_ptr : 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x50 - 0xdf
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX
    stConvert_string, stConvert_obj, 0, stConvert_suid, stConvert_stvar, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX_CONTAINER
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
