#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <cx/debug/assert.h>
#include <cx/platform/cpp.h>
#include <cx/platform/base.h>
#include <cx/utils/macros/salieri.h>
#include <cx/utils/macros/optarg.h>
#include <cx/utils/macros/unused.h>

// extra files that can be included for specific functions
#define STYPE_FOREACH_ALL <cx/stype/alltypes.inc>

CX_C_BEGIN

// do this as a typedef instead of a #define
#undef bool

// IMPORTANT NOTE!
// Always initialize string to NULL or 0 first!
typedef struct str_ref* _Nullable string;
typedef const struct str_ref* _Nullable strref;
typedef struct hashtable_ref* hashtable;
typedef struct closure_ref *closure;
typedef struct cchain_ref *cchain;
typedef struct ObjInst ObjInst;
typedef struct ObjInst_WeakRef ObjInst_WeakRef;
typedef struct SUID SUID;
typedef struct stvar stvar;

enum STYPE_CLASS_ID {
    STCLASS_OPAQUE = 0x00,
    STCLASS_INT = 0x10,
    STCLASS_UINT = 0x20,
    STCLASS_FLOAT = 0x30,
    STCLASS_PTR = 0x40,
    STCLASS_CX = 0xe0,
    STCLASS_CX_CONTAINER = 0xf0,
};

#define STYPE_CLASS_MASK 0xf0
#define STYPE_CLASS(v) ((v) & STYPE_CLASS_MASK)

typedef signed char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;
#ifndef __cplusplus
typedef _Bool bool;
#else
typedef bool _Bool;
#endif

// limits for integer types

#define MIN_INT8    (-0x7f - 1)
#define MAX_INT8      0x7f
#define MAX_UINT8     0xff
#define MIN_INT16   (-0x7fff - 1)
#define MAX_INT16     0x7fff
#define MAX_UINT16    0xffff
#define MIN_INT32   (-0x7fffffffL - 1)
#define MAX_INT32     0x7fffffffL
#define MAX_UINT32    0xffffffffUL
#define MAX_INT64     0x7fffffffffffffffLL
#define MIN_INT64   (-0x7fffffffffffffffLL - 1)
#define MAX_UINT64    0xffffffffffffffffULL

#if defined(_64BIT)
#define MIN_INTPTR MIN_INT64
#define MAX_INTPTR MAX_INT64
#define MAX_UINTPTR MAX_UINT64
#elif defined(_32BIT)
#define MIN_INTPTR MIN_INT32
#define MAX_INTPTR MAX_INT32
#define MAX_UINTPTR MAX_UINT32
#endif

typedef float float32;
typedef double float64;

typedef uint32 stype;

// standardize on uint32 for function call flags
typedef uint32 flags_t;

// sarrays are special because of the pointer union
typedef union sa_ref {
    void *_is_sarray;
    void *a;
} sa_ref;
typedef union sa_ref* sahandle;

// This is the type that is used for passing as a parameter by-value, containers, and
// variants. Should be no larger than 64 bits wide, but can be smaller.
#define SType_none void*
#define SType_opaque void*
#define SType_int8 int8
#define SType_int16 int16
#define SType_int32 int32
#define SType_int64 int64
#define SType_uint8 uint8
#define SType_uint16 uint16
#define SType_uint32 uint32
#define SType_uint64 uint64
#define SType_intptr intptr
#define SType_uintptr uintptr
#define SType_bool _Bool
#define SType_size size_t
#define SType_float32 float32
#define SType_float64 float64
#define SType_ptr void*
#define SType_string string
#define SType_strref strref
#define SType_object ObjInst*
#define SType_weakref ObjInst_WeakRef*
#define SType_suid SUID*
#define SType_stvar stvar*
#define SType_sarray sa_ref
#define SType_hashtable hashtable
#define SType_closure closure
#define SType_cchain cchain
#define stTypeDef(name) SType_##name

#define stTypeCast(name, v) ((SType_##name)(v))
#define stPtrCast(name, v) ((SType_##name*)(v))

// container that can be aliased for any type
#define CONTAINER_TYPE(type) stTypeDef(type) st_##type
typedef union stgeneric {
    uint64 st_generic;
    CONTAINER_TYPE(none);
    CONTAINER_TYPE(opaque);
    CONTAINER_TYPE(int8);
    CONTAINER_TYPE(int16);
    CONTAINER_TYPE(int32);
    CONTAINER_TYPE(int64);
    CONTAINER_TYPE(uint8);
    CONTAINER_TYPE(uint16);
    CONTAINER_TYPE(uint32);
    CONTAINER_TYPE(uint64);
    CONTAINER_TYPE(intptr);
    CONTAINER_TYPE(uintptr);
    CONTAINER_TYPE(bool);
    CONTAINER_TYPE(size);
    CONTAINER_TYPE(float32);
    CONTAINER_TYPE(float64);
    CONTAINER_TYPE(ptr);
    CONTAINER_TYPE(string);
    CONTAINER_TYPE(strref);
    CONTAINER_TYPE(object);
    CONTAINER_TYPE(weakref);
    CONTAINER_TYPE(suid);
    CONTAINER_TYPE(stvar);
    CONTAINER_TYPE(sarray);
    CONTAINER_TYPE(hashtable);
    CONTAINER_TYPE(closure);
    CONTAINER_TYPE(cchain);
} stgeneric;

_Static_assert(sizeof(stgeneric) == sizeof(uint64), "stype container too large");

#ifndef __cplusplus
#define stgeneric(type, val) ((stgeneric){ .st_##type = stCheck(type, val) })
#define stgeneric_unchecked(type, val) ((stgeneric){ .st_##type = (val) })
#define stgensarray(val) stgeneric(ptr, (val)._is_sarray)
#else
#define stgeneric(type, val) ((stgeneric)stCheck(type, val))
#define stgeneric_unchecked(type, val) ((stgeneric)(val))
#define stgensarray(val) stgeneric(ptr, (val)._is_sarray)
#endif

// Compact variant structure. This is most often used for passing arrays of values that
// the type is not known at compile time, as part of the type-safe varargs replacement
// mechanism.

typedef struct stvar {
    stgeneric data;
    stype type;
} stvar;

// The type that's actually used for storage in containers, etc.
#define STStorageType_none void
#define STStorageType_opaque void
#define STStorageType_int8 int8
#define STStorageType_int16 int16
#define STStorageType_int32 int32
#define STStorageType_int64 int64
#define STStorageType_uint8 uint8
#define STStorageType_uint16 uint16
#define STStorageType_uint32 uint32
#define STStorageType_uint64 uint64
#define STStorageType_intptr intptr
#define STStorageType_uintptr uintptr
#define STStorageType_bool _Bool
#define STStorageType_size size_t
#define STStorageType_float32 float32
#define STStorageType_float64 float64
#define STStorageType_ptr void*
#define STStorageType_string string
#define STStorageType_strref string
#define STStorageType_object ObjInst*
#define STStorageType_weakref ObjInst_WeakRef*
#define STStorageType_suid SUID
#define STStorageType_stvar stvar
#define STStorageType_sarray sa_ref
#define STStorageType_hashtable hashtable
#define STStorageType_closure closure
#define STStorageType_cchain cchain
#define stStorageType(name) STStorageType_##name

enum STYPE_ID {
    // none is a special type for empty argument lists, variants, etc
    STypeId_none = STCLASS_OPAQUE,
    // opaque is a magic catch-all type for custom structures and such
    STypeId_opaque = STCLASS_OPAQUE | 1,
    // generic scalar types
    STypeId_int8 = STCLASS_INT | 1,
    STypeId_int16 = STCLASS_INT | 2,
    STypeId_int32 = STCLASS_INT | 4,
    STypeId_int64 = STCLASS_INT | 8,
    STypeId_intptr = STCLASS_INT | sizeof(intptr),  // alias for one of the other int types
    STypeId_uint8 = STCLASS_UINT | 1,
    STypeId_uint16 = STCLASS_UINT | 2,
    STypeId_uint32 = STCLASS_UINT | 4,
    STypeId_uint64 = STCLASS_UINT | 8,
    STypeId_uintptr = STCLASS_UINT | sizeof(intptr),
    STypeId_bool = STCLASS_UINT | 3,                // fill in a gap in the ID sequence
    STypeId_size = STCLASS_UINT | sizeof(size_t),   // alias for one of the uint types
    STypeId_float32 = STCLASS_FLOAT | 4,
    STypeId_float64 = STCLASS_FLOAT | 8,
    // ptr is stored similar to (u)intptr, but automatically dereferenced in ops functions
    STypeId_ptr = STCLASS_PTR | sizeof(void*),
    // most of the CX class are "object-like" types and use pointers as handles
    STypeId_string = STCLASS_CX | 0,
    STypeId_strref = STCLASS_CX | 0,
    STypeId_object = STCLASS_CX | 1,
    STypeId_weakref = STCLASS_CX | 2,
    STypeId_suid = STCLASS_CX | 3,        // notable exception
    STypeId_stvar = STCLASS_CX | 4,
    STypeId_closure = STCLASS_CX | 5,
    STypeId_sarray = STCLASS_CX_CONTAINER | 0,
    STypeId_hashtable = STCLASS_CX_CONTAINER | 1,
    STypeId_cchain = STCLASS_CX_CONTAINER | 2,
};
#define stTypeId(name) STypeId_##name

// The actual storage size of the type
enum STYPE_SIZE {
    STypeSize_none = 0,
    // opaque is not really size 0, but filled by by macros later
    STypeSize_opaque = 0,
    STypeSize_int8 = sizeof(int8),
    STypeSize_int16 = sizeof(int16),
    STypeSize_int32 = sizeof(int32),
    STypeSize_int64 = sizeof(int64),
    STypeSize_intptr = sizeof(intptr),
    STypeSize_uint8 = sizeof(int8),
    STypeSize_uint16 = sizeof(int16),
    STypeSize_uint32 = sizeof(int32),
    STypeSize_uint64 = sizeof(int64),
    STypeSize_uintptr = sizeof(uintptr),
    STypeSize_bool = sizeof(_Bool),
    STypeSize_size = sizeof(size_t),
    STypeSize_float32 = sizeof(float32),
    STypeSize_float64 = sizeof(float64),
    STypeSize_ptr = sizeof(void*),
    STypeSize_string = sizeof(void*),
    STypeSize_strref = sizeof(void*),
    STypeSize_object = sizeof(ObjInst*),
    STypeSize_weakref = sizeof(ObjInst_WeakRef*),
    // SUID is special because it's always passed by reference, but stored as the full 16 bytes
    STypeSize_suid = 16,
    STypeSize_stvar = sizeof(stvar),
    STypeSize_sarray = sizeof(sa_ref),
    STypeSize_hashtable = sizeof(hashtable),
    STypeSize_closure = sizeof(closure),
    STypeSize_cchain = sizeof(cchain),
};
#define stTypeSize(name) STypeSize_##name

enum STYPE_FLAGS {
    STypeFlag_Object =   (1 << 0),      // "object-like" type -- pointer to managed object
    STypeFlag_Custom =   (1 << 1),      // type uses custom ops
    STypeFlag_PassPtr =  (1 << 2),      // type is passed by pointer rather than by value,
                                        // does not apply to 'handle' style objects
};
#define stFlag(name) STypeFlag_##name
#define stHasFlag(st, fname) ((st >> 8) & stFlag(fname))

/*typedef union stype {
    uint32 spec;
    struct {
        uint8 id;
        uint8 flags;
        uint16 size;
    };
} stype; */
#define stGetId(st) (st & 0xff)
#define stGetFlags(st) ((st >> 8) & 0xff)
#define stGetSize(st) (st >> 16)

#define _stype_mktype(tid, tflags, tsz) ((tid & 0xff) | (tflags & 0xff) << 8 | (tsz & 0xffff) << 16)

// ignore custom flag when comparing types
_meta_inline bool stEq(stype sta, stype stb) {
    return (sta & ~_stype_mktype(0, STypeFlag_Custom, 0)) == (stb & ~_stype_mktype(0, STypeFlag_Custom, 0));
}

enum STYPE_DEFAULT_FLAGS {
    STypeFlags_none = 0,
    STypeFlags_opaque = stFlag(PassPtr),
    STypeFlags_int8 = 0,
    STypeFlags_int16 = 0,
    STypeFlags_int32 = 0,
    STypeFlags_int64 = 0,
    STypeFlags_intptr = 0,
    STypeFlags_uint8 = 0,
    STypeFlags_uint16 = 0,
    STypeFlags_uint32 = 0,
    STypeFlags_uint64 = 0,
    STypeFlags_uintptr = 0,
    STypeFlags_bool = 0,
    STypeFlags_size = 0,
    STypeFlags_float32 = 0,
    STypeFlags_float64 = 0,
    STypeFlags_ptr = 0,
    STypeFlags_string = stFlag(Object),
    STypeFlags_strref = stFlag(Object),
    STypeFlags_object = stFlag(Object),
    STypeFlags_weakref = stFlag(Object),
    STypeFlags_suid = stFlag(PassPtr),
    STypeFlags_stvar = stFlag(PassPtr) | stFlag(Object),
    STypeFlags_sarray = stFlag(Object),
    STypeFlags_hashtable = stFlag(Object),
    STypeFlags_closure = stFlag(Object),
    STypeFlags_cchain = stFlag(Object),
};
#define stTypeFlags(name) STypeFlags_##name

#define stTypeInternal(name) _stype_mktype(stTypeId(name), stTypeFlags(name), stTypeSize(name))
#define stFullTypeInternal(name) stTypeInternal(name), 0
_meta_inline stype _stype_mkcustom(stype base)
{
    base |= _stype_mktype(0, stFlag(Custom), 0);
    return base;
}

// Static type checks
// These types all include the marker as either the first member, or as part of
// a union that coincides with the first member. Therefore we can take their address
// and get the address of the parent structure that we already had (essentially a no-op)
// while checking that they exist, thus verifying the type is correct.
// Note the magic of the comma operator -- these prevent compilation if something is wrong,
// but get optimized away entirely and do not emit any code.
// The use of unused_noeval further improves this by hiding it in a conditional branch that
// is guaranteed to not be evaluated, preventing any side effects such as function calls or
// postfix operators.
// The seemingly redundant (h) && portions of the expression are there solely to suppress
// warnings from stupid compilers that don't understand it can never actually be dereferenced.
#define saCheckType(name, h) ((sa_##name*)(unused_noeval((h) && &((h)->is_sarray_##name)), (h)))
#define saCheck(s) (unused_noeval(&((s)._is_sarray)), (s.a))
#define saCheckPtr(h) (unused_noeval((h != NULL) && &((h)->_is_sarray)), (h))
#define htCheck(h) (unused_noeval((h) && &((h)->_is_hashtable)), (h))
#define htCheckPtr(h) (unused_noeval((h) && (*h) && &((*h)->_is_hashtable)), (h))
#define objInstCheck(o) (unused_noeval((o) && &((o)->_is_ObjInst)), (o))
#define objInstCheckPtr(o) (unused_noeval((o != NULL) && (*o) && &((*o)->_is_ObjInst)), (o))
#define objWeakRefCheck(o) (unused_noeval((o) && &((o)->_is_ObjInst_WeakRef)), (o))
#define objWeakRefCheckPtr(o) (unused_noeval((o) && (*o) && &((*o)->_is_ObjInst_WeakRef)), (o))
#define strCheck(s) (unused_noeval((s) && &((s)->_is_string)), (s))
#define strCheckPtr(s) (unused_noeval((s != NULL) && (*s) && &((*s)->_is_string)), (s))
#define closureCheck(c) (unused_noeval((c) && &((c)->_is_closure)), (c))
#define closureCheckPtr(c) (unused_noeval((c != NULL) && (*c) && &((*c)->_is_closure)), (c))
#define cchainCheck(c) (unused_noeval((c) && &((c)->_is_closure_chain)), (c))
#define cchainCheckPtr(c) (unused_noeval((c != NULL) && (*c) && &((*c)->_is_closure_chain)), (c))

// most of these are no-ops, but some can do extra type checking
#define STypeCheck_opaque(type, val) (val)
#define STypeCheck_int8(type, val) (val)
#define STypeCheck_int16(type, val) (val)
#define STypeCheck_int32(type, val) (val)
#define STypeCheck_int64(type, val) (val)
#define STypeCheck_intptr(type, val) (val)
#define STypeCheck_uint8(type, val) (val)
#define STypeCheck_uint16(type, val) (val)
#define STypeCheck_uint32(type, val) (val)
#define STypeCheck_uint64(type, val) (val)
#define STypeCheck_uintptr(type, val) (val)
#define STypeCheck_bool(type, val) (val)
#define STypeCheck_size(type, val) (val)
#define STypeCheck_float32(type, val) (val)
#define STypeCheck_float64(type, val) (val)
#define STypeCheck_ptr(type, val) (val)
#define STypeCheck_string(type, val) strCheck(val)
#define STypeCheck_strref(type, val) strCheck(val)
#define STypeCheck_object(type, val) objInstCheck(val)
#define STypeCheck_weakref(type, val) objWeakRefCheck(val)
#define STypeCheck_suid(type, val) (unused_noeval((((val).low), ((val).high))), (val))
#define STypeCheck_stvar(_type, val) (unused_noeval((((val).data), ((val).type))), (val))
#define STypeCheck_sarray(type, val) saCheck(val)
#define STypeCheck_hashtable(type, val) htCheck(val)
#define STypeCheck_closure(type, val) closureCheck(val)
#define STypeCheck_cchain(type, val) cchainCheck(val)
#define stCheck(type, val) STypeCheck_##type(type, val)

#define STypeCheckPtr_gen(type, ptr) (unused_noeval((stTypeDef(type)*)(ptr)), ptr)
#define STypeCheckPtr_opaque(type, ptr) (ptr)
#define STypeCheckPtr_int8(type, ptr) (ptr)
#define STypeCheckPtr_int16(type, ptr) (ptr)
#define STypeCheckPtr_int32(type, ptr) (ptr)
#define STypeCheckPtr_int64(type, ptr) (ptr)
#define STypeCheckPtr_intptr(type, ptr) (ptr)
#define STypeCheckPtr_uint8(type, ptr) (ptr)
#define STypeCheckPtr_uint16(type, ptr) (ptr)
#define STypeCheckPtr_uint32(type, ptr) (ptr)
#define STypeCheckPtr_uint64(type, ptr) (ptr)
#define STypeCheckPtr_uintptr(type, ptr) (ptr)
#define STypeCheckPtr_bool(type, ptr) (ptr)
#define STypeCheckPtr_size(type, ptr) (ptr)
#define STypeCheckPtr_float32(type, ptr) (ptr)
#define STypeCheckPtr_float64(type, ptr) (ptr)
#define STypeCheckPtr_ptr(type, ptr) (ptr)
#define STypeCheckPtr_string(type, ptr) strCheckPtr(ptr)
#define STypeCheckPtr_strref(type, ptr) strCheckPtr(ptr)
#define STypeCheckPtr_object(type, ptr) objInstCheckPtr(ptr)
#define STypeCheckPtr_weakref(type, ptr) objWeakRefCheckPtr(ptr)
#define STypeCheckPtr_suid(type, ptr) (unused_noeval((((ptr)->low), ((ptr)->high))), (ptr))
#define STypeCheckPtr_stvar(_type, ptr) (unused_noeval((((ptr)->data), ((ptr)->type))), (ptr))
#define STypeCheckPtr_sarray(type, ptr) saCheckPtr(ptr)
#define STypeCheckPtr_hashtable(type, ptr) htCheckPtr(ptr)
#define STypeCheckPtr_closure(type, ptr) closureCheckPtr(ptr)
#define STypeCheckPtr_cchain(type, ptr) cchainCheckPtr(ptr)
#define stCheckPtr(type, ptr) STypeCheckPtr_##type(type, ptr)

// C99 compound literals are lvalues and can force the compiler to create a temporary
// on the stack if necessary, so that we can pass pointers to arbitrary expressions
#define stRvalAddr(type, rval) ((stStorageType(type)[1]){rval})

// MEGA PREPROCESSOR HACKS INCOMING
#define stType_none stTypeInternal(none)
// this enables the use of opaque(realtype) as type name in functions like saCreate
#define stType_opaque(realtype) _stype_mktype(stTypeId(opaque), stTypeFlags(opaque), (uint16)sizeof(realtype))
#define stType_int8 stTypeInternal(int8)
#define stType_int16 stTypeInternal(int16)
#define stType_int32 stTypeInternal(int32)
#define stType_int64 stTypeInternal(int64)
#define stType_intptr stTypeInternal(intptr)
#define stType_uint8 stTypeInternal(uint8)
#define stType_uint16 stTypeInternal(uint16)
#define stType_uint32 stTypeInternal(uint32)
#define stType_uint64 stTypeInternal(uint64)
#define stType_uintptr stTypeInternal(uintptr)
#define stType_bool stTypeInternal(bool)
#define stType_size stTypeInternal(size)
#define stType_float32 stTypeInternal(float32)
#define stType_float64 stTypeInternal(float64)
#define stType_ptr stTypeInternal(ptr)
#define stType_string stTypeInternal(string)
#define stType_strref stTypeInternal(strref)
#define stType_object stTypeInternal(object)
#define stType_weakref stTypeInternal(weakref)
#define stType_suid stTypeInternal(suid)
#define stType_stvar stTypeInternal(stvar)
#define stType_sarray stTypeInternal(sarray)
#define stType_hashtable stTypeInternal(hashtable)
#define stType_closure stTypeInternal(closure)
#define stType_cchain stTypeInternal(cchain)
#define stType_custom(basetype) _stype_mkcustom(stType_##basetype)
#define stType(name) stType_##name

#define stFullType_none stFullTypeInternal(none)
// and the hack for custom(basetype, ops)
#define stFullType_opaque(realtype) _stype_mktype(stTypeId(opaque), stTypeFlags(opaque), (uint16)sizeof(realtype)), 0
#define stFullType_int8 stFullTypeInternal(int8)
#define stFullType_int16 stFullTypeInternal(int16)
#define stFullType_int32 stFullTypeInternal(int32)
#define stFullType_int64 stFullTypeInternal(int64)
#define stFullType_intptr stFullTypeInternal(intptr)
#define stFullType_uint8 stFullTypeInternal(uint8)
#define stFullType_uint16 stFullTypeInternal(uint16)
#define stFullType_uint32 stFullTypeInternal(uint32)
#define stFullType_uint64 stFullTypeInternal(uint64)
#define stFullType_uintptr stFullTypeInternal(uintptr)
#define stFullType_bool stFullTypeInternal(bool)
#define stFullType_size stFullTypeInternal(size)
#define stFullType_float32 stFullTypeInternal(float32)
#define stFullType_float64 stFullTypeInternal(float64)
#define stFullType_ptr stFullTypeInternal(ptr)
#define stFullType_string stFullTypeInternal(string)
#define stFullType_strref stFullTypeInternal(strref)
#define stFullType_object stFullTypeInternal(object)
#define stFullType_weakref stFullTypeInternal(weakref)
#define stFullType_suid stFullTypeInternal(suid)
#define stFullType_stvar stFullTypeInternal(stvar)
#define stFullType_sarray stFullTypeInternal(sarray)
#define stFullType_hashtable stFullTypeInternal(hashtable)
#define stFullType_closure stFullTypeInternal(closure)
#define stFullType_cchain stFullTypeInternal(cchain)
// this will chain evaluate macros for stuff like custom(opaque(realtype), ops)
// it gets token pasted as _stype_mkcustom(stType_opaque(realtype), ops)
#define stFullType_custom(basetype, ops) _stype_mkcustom(stType_##basetype), (&ops)
#define stFullType(name) stFullType_##name

// Macros for passing arguments by value

// none ignores the value
#define STypeArg_none(type, val) ((stgeneric){ 0 })
// opqaue is a special case that must always be passed by pointer, and
// the caller must supply an lvalue
#define STypeArg_opaque(type, val) stgeneric(type, &(val))
// but everything else gets put into a container
#define STypeArg_int8(type, val) stgeneric(type, val)
#define STypeArg_int16(type, val) stgeneric(type, val)
#define STypeArg_int32(type, val) stgeneric(type, val)
#define STypeArg_int64(type, val) stgeneric(type, val)
#define STypeArg_intptr(type, val) stgeneric(type, val)
#define STypeArg_uint8(type, val) stgeneric(type, val)
#define STypeArg_uint16(type, val) stgeneric(type, val)
#define STypeArg_uint32(type, val) stgeneric(type, val)
#define STypeArg_uint64(type, val) stgeneric(type, val)
#define STypeArg_uintptr(type, val) stgeneric(type, val)
#define STypeArg_bool(type, val) stgeneric(type, val)
#define STypeArg_size(type, val) stgeneric(type, val)
#define STypeArg_float32(type, val) stgeneric(type, val)
#define STypeArg_float64(type, val) stgeneric(type, val)
#define STypeArg_ptr(type, val) stgeneric(type, val)
#define STypeArg_string(type, val) stgeneric(type, val)
#define STypeArg_strref(type, val) stgeneric(type, val)
#define STypeArg_object(type, val) stgeneric(type, objInstBase(val))
#define STypeArg_weakref(type, val) stgeneric(type, objWeakRefBase(val))
// SUID and stvar are too big, so make a copy and pass a pointer
#define STypeArg_suid(type, val) stgeneric_unchecked(type, stRvalAddr(type, stCheck(type, val)))
#define STypeArg_stvar(type, val) stgeneric_unchecked(type, stRvalAddr(type, stCheck(type, val)))
#define STypeArg_sarray(type, val) stgensarray(val)
#define STypeArg_hashtable(type, val) stgeneric(type, val)
#define STypeArg_closure(type, val) stgeneric(type, val)
#define STypeArg_cchain(type, val) stgeneric(type, val)
#define stArg(type, val) STypeArg_##type(type, val)

// And for passing a pointer-to-pointer, mostly for functions that want to
// consume or reallocate the object

#define STypeArgPtr_none(type, val) NULL
// opaque is already passed in as a pointer
#define STypeArgPtr_opaque(type, val) &stgeneric(type, val)
#define STypeArgPtr_int8(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_int16(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_int32(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_int64(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_intptr(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_uint8(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_uint16(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_uint32(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_uint64(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_uintptr(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_bool(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_size(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_float32(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_float64(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_ptr(type, val) (stgeneric*)stCheckPtr(type, (void**)(val))
#define STypeArgPtr_string(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_strref(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_object(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_weakref(type, val) (stgeneric*)stCheckPtr(type, val)
// same for the other pass-by-pointer cases (SUID, stvar)
#define STypeArgPtr_suid(type, val) &stgeneric_unchecked(type, stCheckPtr(type, val))
#define STypeArgPtr_stvar(type, val) &stgeneric_unchecked(type, stCheckPtr(type, val))
#define STypeArgPtr_sarray(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_hashtable(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_closure(type, val) (stgeneric*)stCheckPtr(type, val)
#define STypeArgPtr_cchain(type, val) (stgeneric*)stCheckPtr(type, val)
#define stArgPtr(type, val) STypeArgPtr_##type(type, val)

// Macros for type-checked inline metafunctions.
// These expand to a pair of parameters for type, followed by a pointer.

#define STypeCheckedArg_none(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_opaque(type, val) stType_opaque(val), stArg(type, val)
#define STypeCheckedArg_int8(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_int16(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_int32(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_int64(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_intptr(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_uint8(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_uint16(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_uint32(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_uint64(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_uintptr(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_bool(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_size(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_float32(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_float64(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_ptr(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_string(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_strref(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_object(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_weakref(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_suid(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_stvar(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_sarray(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_hashtable(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_closure(type, val) stTypeInternal(type), stArg(type, val)
#define STypeCheckedArg_cchain(type, val) stTypeInternal(type), stArg(type, val)
#define stCheckedArg(type, val) STypeCheckedArg_##type(type, val)

// Type checking of pointers to types, mostly for functions that want to
// consume object-like variables and destroy them

#define STypeCheckedPtrArg_none(type, val) stTypeInternal(type), stArgPtr(type, val)
// go the opposite direction for opaque since it's already passed in as a pointer
#define STypeCheckedPtrArg_opaque(type, val) stType_opaque(*val), stArgPtr(type, val)
#define STypeCheckedPtrArg_int8(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_int16(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_int32(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_int64(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_intptr(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_uint8(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_uint16(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_uint32(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_uint64(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_uintptr(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_bool(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_size(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_float32(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_float64(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_ptr(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_string(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_strref(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_object(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_weakref(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_suid(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_stvar(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_sarray(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_hashtable(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_closure(type, val) stTypeInternal(type), stArgPtr(type, val)
#define STypeCheckedPtrArg_cchain(type, val) stTypeInternal(type), stArgPtr(type, val)
#define stCheckedPtrArg(type, val) STypeCheckedPtrArg_##type(type, val)

enum STYPE_OPS_FLAGS {
    // Valid for: cmp, hash
    // Perform a case-insensitive hash or compare operation if the
    // underlying type supports it
    ST_CaseInsensitive  = 0x00000001,

    // Valid for: cmp
    // Indicates that the caller only cares about equality and not ordering. Cmp is
    // allowed to return 0 for equal and any nonzero value for not equal. This is mostly
    // important for objects that do not implement the Sortable interface to be able to
    // be compared, especially when inside stvar or similar.
    ST_Equality         = 0x00000002,

    // Valid for: convert
    // Conversion normally checks the range of the destination type and the conversion
    // will fail if it would fall outside the range that a value can be represented.
    // This flag disables range checking and allows overflow/underflow. This may result
    // in sign flips for signed integers.
    ST_Overflow         = 0x00000004,

    // Valid for: convert
    // Some types cannot accurately represent all values within their range. For example,
    // not all 32-bit integers can be uniquely represented by a 32-bit float despite being
    // well within the numeric range.
    // This flag enables extra checks that will fail the conversion if it could not be
    // uniquely reversed.
    ST_Lossless         = 0x00000008,
};

// Implementation notes:
// stype ops are LOW LEVEL operations for dealing with data types at the most basic level.
// In particular, stCopy and stConvert will OVERWRITE the destination without checking.
// This allows them to be used to efficiently initialize otherwise initialized memory,
// but caution should be used with higher level types to avoid leaking memory.
// For example, using stCopy to overwrite an existing string or sarray will leak the
// destination because stCopy does not destroy the existing object first.

#define _stCopyDest_Anno_(typvar) _Post_valid_ _When_(stHasFlag(typvar, PassPtr), _Pre_valid_)
#define _stCopyDest_Anno_opt_(typvar) _Out_opt_ _When_(stHasFlag(typvar, PassPtr), _Pre_opt_valid_)

typedef void (*stDtorFunc)(stype st, _Pre_notnull_ _Post_invalid_ stgeneric* gen, flags_t flags);
typedef intptr (*stCmpFunc)(stype st, _In_ stgeneric gen1, _In_ stgeneric gen2, flags_t flags);
typedef uint32 (*stHashFunc)(stype st, _In_ stgeneric gen, flags_t flags);
typedef void (*stCopyFunc)(stype st, _stCopyDest_Anno_(st) stgeneric* dest, _In_ stgeneric src, flags_t flags);
typedef _Success_(return) _Check_return_ bool (*stConvertFunc)(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_ stgeneric src, flags_t flags);

extern stDtorFunc _stDefaultDtor[256];
extern stCmpFunc _stDefaultCmp[256];
extern stHashFunc _stDefaultHash[256];
extern stCopyFunc _stDefaultCopy[256];
extern stConvertFunc _stDefaultConvert[256];

typedef struct STypeOps {
    stDtorFunc dtor;
    stCmpFunc cmp;
    stHashFunc hash;
    stCopyFunc copy;
    stConvertFunc convert;
} STypeOps;

_meta_inline stgeneric _stStoredVal(stype st, _In_ const void *storage)
{
    stgeneric ret;
    switch (stGetSize(st)) {
    case 1:
        ret.st_uint8 = *(uint8*)storage;
        break;
    case 2:
        ret.st_uint16 = *(uint16*)storage;
        break;
    case 4:
        ret.st_uint32 = *(uint32*)storage;
        break;
    case 8:
        ret.st_uint64 = *(uint64*)storage;
        break;
    default:
        devFatalError("Invalid small stype size");
        ret.st_uint64 = 0;
    }
    return ret;
}

#define stStored(st, storage) (stHasFlag(st, PassPtr) ? stgeneric(ptr, (void*)(storage)) : _stStoredVal(st, storage))
#define stStoredPtr(st, storage) (stHasFlag(st, PassPtr) ? &stgeneric(ptr, ((void*)(storage))) : (stgeneric*)((void*)(storage)))

#define stGenPtr(st, gen) (stHasFlag(st, PassPtr) ? (gen).st_ptr : &(gen))

// inlining these lets most of it get optimized out and specialized if the type is known at compile-time

_meta_inline void _stDestroy(stype st, _In_opt_ STypeOps *ops, _Pre_notnull_ _Post_invalid_ stgeneric *gen, flags_t flags)
{
    // ops is mandatory for custom type
    devAssert(!stHasFlag(st, Custom) || ops);

    if (ops && ops->dtor)
        ops->dtor(st, gen, flags);
    else if (_stDefaultDtor[stGetId(st)])
        _stDefaultDtor[stGetId(st)](st, gen, flags);
}
#define stDestroy(type, pobj, ...) _stDestroy(stFullType(type), stArgPtr(type, pobj), opt_flags(__VA_ARGS__))

_meta_inline intptr _stCmp(stype st, _In_opt_ STypeOps *ops, _In_ stgeneric gen1, _In_ stgeneric gen2, flags_t flags)
{
    // ops is mandatory for custom type
    devAssert(!stHasFlag(st, Custom) || ops);

    if (ops && ops->cmp)
        return ops->cmp(st, gen1, gen2, flags);

    if (_stDefaultCmp[stGetId(st)])
        return _stDefaultCmp[stGetId(st)](st, gen1, gen2, flags);

    if (!stHasFlag(st, PassPtr))
        return memcmp(&gen1, &gen2, stGetSize(st));
    else
        return memcmp(gen1.st_ptr, gen2.st_ptr, stGetSize(st));
}
#define stCmp(type, obj1, obj2, ...) _stCmp(stFullType(type), stArg(type, obj1), stArg(type, obj2), opt_flags(__VA_ARGS__))

_meta_inline void _stCopy(stype st, _In_opt_ STypeOps *ops, _stCopyDest_Anno_(st) stgeneric *dest, _In_ stgeneric src, flags_t flags)
{
    // ops is mandatory for custom type
    devAssert(!stHasFlag(st, Custom) || ops);

    stCopyFunc defCopy = _stDefaultCopy[stGetId(st)];

    if (ops && ops->copy)
        ops->copy(st, dest, src, flags);
    else if (defCopy)
        defCopy(st, dest, src, flags);
    else if (!stHasFlag(st, PassPtr))
        memcpy(dest, &src, stGetSize(st));
    else
        memcpy(dest->st_ptr, src.st_ptr, stGetSize(st));
}
#define stCopy(type, pdest, src, ...) _stCopy(stFullType(type), stArgPtr(type, pdest), stArg(type, src), opt_flags(__VA_ARGS__))

uint32 stHash_gen(stype st, _In_ stgeneric stgen, flags_t flags);
_meta_inline uint32 _stHash(stype st, _In_opt_ STypeOps *ops, _In_ stgeneric gen, flags_t flags)
{
    // ops is mandatory for custom type
    devAssert(!stHasFlag(st, Custom) || ops);

    if (ops && ops->hash)
        return ops->hash(st, gen, flags);
    else if (_stDefaultHash[stGetId(st)])
        return _stDefaultHash[stGetId(st)](st, gen, flags);
    else
        return stHash_gen(st, gen, flags);
}
#define stHash(type, obj, ...) _stHash(stFullType(type), stArg(type, obj), opt_flags(__VA_ARGS__))

_Success_(return) _Check_return_
_meta_inline bool _stConvert(stype destst, _stCopyDest_Anno_(destst) stgeneric *dest, stype srcst, _In_opt_ STypeOps *srcops, _In_ stgeneric src, flags_t flags)
{
    // ops is mandatory for custom type
    devAssert(!stHasFlag(srcst, Custom) || srcops);

    stConvertFunc defConvert = _stDefaultConvert[stGetId(srcst)];

    // The *source* stype is responsible for handling conversions to other types
    if (srcops && srcops->convert)
        return srcops->convert(destst, dest, srcst, src, flags);
    else if (srcst == destst)
        return _stCopy(destst, NULL, dest, src, flags), true;
    else if (defConvert)
        return defConvert(destst, dest, srcst, src, flags);
    else
        return false;               // can't convert it if we don't know how!
}
#define stConvert(desttype, pdest, srctype, src, ...) _stConvert(stType(desttype), stArgPtr(desttype, pdest), stFullType(srctype), stArg(srctype, src), opt_flags(__VA_ARGS__))

CX_C_END
