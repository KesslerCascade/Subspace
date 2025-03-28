#include "cx/cx.h"
#include "cx/utils/murmur.h"

#include "cx/obj/stype_obj.h"
#include "cx/container/stype_sarray.h"
#include "cx/string/stype_string.h"
#include "cx/container/stype_hashtable.h"
#include "cx/stype/stype_stvar.h"
#include "cx/suid/stype_suid.h"
#include "cx/closure/stype_closure.h"

#include "cx/debug/dbgtypes.h"

// extern inline stype _stype_mktype(uint8 id, uint8 flags, uint16 sz);
// extern inline void stDestroy(stype st, void *ptr, STypeOps *ops);
// extern inline intptr stCmp(stype st, const void *ptr1, const void *ptr2, STypeOps *ops);
// extern inline void stCopy(stype st, void *dest, const void *src, STypeOps *ops);

#define STCMP_GEN(type) \
static intptr stCmp_##type(stype st, stgeneric gen1, stgeneric gen2, uint32 flags) \
{ \
    type tmp = gen1.st_##type - gen2.st_##type; \
    return (intptr)tmp; \
}
#define STCMP_GEN_OVR(type, ovrtype) \
static intptr stCmp_##type(stype st, stgeneric gen1, stgeneric gen2, uint32 flags) \
{ \
    return (intptr)((ovrtype)gen1.st_##type - (ovrtype)gen2.st_##type); \
}

STCMP_GEN(int8)
STCMP_GEN(int16)
STCMP_GEN(int32)
STCMP_GEN(uint8)
STCMP_GEN(uint16)
STCMP_GEN(uint32)
#ifdef _64BIT
STCMP_GEN(int64)
STCMP_GEN(uint64)
#else
static intptr stCmp_int64(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    if(gen1.st_int64 == gen2.st_int64)
        return 0;
    return (gen1.st_int64 < gen2.st_int64) ? -1 : 1;
}

static intptr stCmp_uint64(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    if(gen1.st_uint64 == gen2.st_uint64)
        return 0;
    return (gen1.st_uint64 < gen2.st_uint64) ? -1 : 1;
}
#endif

// 'equal enough' values good enough for general use while taking into account small
// amounts of floating-point error.
//
// Notably this does NOT handle the problem of subtracting numbers that are very close
// together, perhaps only 1 representable float apart, and ending up with a value that
// is near zero but also a long way away in floating point space due to having more
// precision closer to zero.
//
// stCmp is mostly used for things like sorting lists or detecting changes, so this is
// good enough to reach a stable result.
// 
// Specialized applications will need to decide their own tolerance for comparison

// Implementation note: Checks for equality early to handle NaN and Inf, then uses
// integer representation which in IEEE-754 helpfully means that representable floats
// right next to each other in FP space are also adjacent in integer space because
// the mantissa comes last.

#define STCMP_FLOAT_ULP_EPSILON (2)

#define STCMP_FLOAT(type, itype) \
static intptr stCmp_##type(stype st, stgeneric gen1, stgeneric gen2, uint32 flags) \
{ \
    if (gen1.st_##type == gen2.st_##type) \
        return 0; \
    itype idiff = gen1.st_##itype - gen2.st_##itype; \
    if (idiff <= STCMP_FLOAT_ULP_EPSILON && idiff >= -STCMP_FLOAT_ULP_EPSILON) \
        return 0; \
    return (gen1.st_##type < gen2.st_##type) ? -1 : 1; \
}
STCMP_FLOAT(float32, int32)
STCMP_FLOAT(float64, int64)

// compiler is stupid about void* comparisons
STCMP_GEN_OVR(ptr, char*)
STCMP_GEN_OVR(hashtable, char*)

static intptr stCmp_sarray(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    return (intptr)((char*)gen1.st_sarray.a - (char*)gen2.st_sarray.a);
}

uint32 stHash_gen(stype st, _In_ stgeneric gen, flags_t flags)
{
    if (!stHasFlag(st, PassPtr))
        return hashMurmur3((uint8*)&gen, stGetSize(st));
    else
        return hashMurmur3(gen.st_ptr, stGetSize(st));
}

static intptr stCmp_none(stype st, stgeneric gen1, stgeneric gen2, uint32 flags)
{
    // none is never equal to anything, even itself
    return 1;
}

static uint32 stHash_none(stype st, stgeneric gen, uint32 flags)
{
    return 0;
}

static void stCopy_none(stype st, _stCopyDest_Anno_(st) stgeneric* dest, _In_ stgeneric src,
                        flags_t flags)
{
    // Ensure that the debug types are always available.
    // This spot was chosen because it's in a function that is virtually impossible to be omitted
    // during linking.
    relAssertMsg(_unused_debug_types._unused == 0, "Memory corruption");
}

alignMem(64) stDtorFunc _stDefaultDtor[256] = {
    // STCLASS_OPAQUE
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_INT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_UINT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_FLOAT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_PTR
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    stDtor_string, stDtor_obj, stDtor_weakref, 0, stDtor_stvar, stDtor_closure, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX_CONTAINER
    stDtor_sarray, stDtor_hashtable, stDtor_cchain, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

alignMem(64) stCmpFunc _stDefaultCmp[256] = {
    // STCLASS_OPAQUE
    stCmp_none, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_INT
    0, stCmp_int8, stCmp_int16, 0, stCmp_int32, 0, 0, 0, stCmp_int64, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_UINT
    0, stCmp_uint8, stCmp_uint16, 0, stCmp_uint32, 0, 0, 0, stCmp_uint64, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_FLOAT
    0, 0, 0, 0, stCmp_float32, 0, 0, 0, stCmp_float64, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_PTR
    0, 0, 0, 0, sizeof(void*) == sizeof(int32) ? stCmp_ptr : 0, 0, 0, 0,
    sizeof(void*) == sizeof(int64) ? stCmp_ptr : 0, 0, 0, 0, 0, 0, 0, 0,
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
    stCmp_string, stCmp_obj, stCmp_weakref, stCmp_suid, stCmp_stvar, stCmp_closure, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX_CONTAINER
    stCmp_sarray, stCmp_hashtable, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

alignMem(64) stHashFunc _stDefaultHash[256] = {
    // STCLASS_OPAQUE
    stHash_none, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_INT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_UINT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_FLOAT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_PTR
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    stHash_string, stHash_obj, 0, 0, stHash_stvar, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX_CONTAINER
    stHash_sarray, stHash_hashtable, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

alignMem(64) stCopyFunc _stDefaultCopy[256] = {
    // STCLASS_OPAQUE
    stCopy_none, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_INT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_UINT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_FLOAT
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_PTR
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    stCopy_string, stCopy_obj, stCopy_weakref, 0, stCopy_stvar, stCopy_closure, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // STCLASS_CX_CONTAINER
    stCopy_sarray, stCopy_hashtable, stCopy_cchain, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
