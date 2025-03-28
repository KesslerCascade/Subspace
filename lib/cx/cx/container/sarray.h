#pragma once

#include <cx/cx.h>
#include <cx/debug/assert.h>
#include <cx/debug/dbgtypes.h>
#include <cx/utils/macros/unused.h>

#define sarrayref(typ) sa_##typ
#define sarrayhdl(typ) sa_##typ*

// for creating a named sarray type that can be passed between functions
#define saDeclareType(name, typ) typedef union sa_##name { \
    _nv_sarray *_debug; \
    void *_is_sarray; \
    void *_is_sarray_##name; \
    typ *a; \
} sa_##name
#define saDeclare(name) saDeclareType(name, name)
#define saDeclarePtr(name) saDeclareType(name, name*)
#define saInitNone { .a = 0 }

// sarray type declarations
typedef sa_ref sa_opaque;
saDeclare(int8);
saDeclare(int16);
saDeclare(int32);
saDeclare(int64);
saDeclare(intptr);
saDeclare(uint8);
saDeclare(uint16);
saDeclare(uint32);
saDeclare(uint64);
saDeclare(uintptr);
saDeclare(bool);
saDeclareType(size, size_t);
saDeclare(float32);
saDeclare(float64);
saDeclareType(ptr, void*);
saDeclare(string);
// strref doesn't make sense in an sarray
saDeclareType(object, ObjInst*);
saDeclareType(suid, SUID);
saDeclare(stvar);
saDeclareType(sarray, sa_ref);
saDeclare(hashtable);

typedef struct SArrayHeader {
    // sarray extended header begins here (only valid if SAINT_Extended is set)
    STypeOps typeops;
    // sarray header begins here
    stype elemtype;
    int32 count;
    int32 capacity;
    uint32 flags;     // high 8 bits = growth
    void *data[1];
} SArrayHeader;

enum SARRAY_CREATE_FLAGS_ENUM
{
    SA_Ref          = 0x0010,   // the array references other data and does not copy/own/destroy it.
                                // only has an effect on pointer or object-type arrays
    SA_Sorted       = 0x0020,   // array is maintined in sorted order
                                // O(log n) search and O(n) insert
    SA_AutoShrink   = 0x0040,   // release memory when number of array elements shrinks

    // internal use only, do not set manually
    SAINT_Extended  = 0x8000,   // includes extended header
};

enum SARRAY_GROW_ENUM {
    // automatically select based on element size
    SA_GROW_Auto,

    // dynamic growth    // 100% - 50% - 25%
    SA_GROW_Normal,      //  16  - 128
    SA_GROW_Aggressive,  //  32  - 256
    SA_GROW_Slow,        //   8  -  64

    // fixed growth
    SA_GROW_100,         // always grow by 100%
    SA_GROW_50,          // 50%
    SA_GROW_25,          // 25%

    // always grow to exact size needed, never allocate extra
    SA_GROW_Minimal,
};

enum SARRAY_FUNC_FLAGS_ENUM {
    // Valid for: saPush, saMerge
    // Does not insert if a matching element already exists.
    SA_Unique           = 0x00010000,

    // Valid for: saExtract, saRemove, saFindRemove
    // When removing/destroying an element, swaps with last element,
    // disrupting the order but removing the element much faster.
    // Not valid for sorted arrays.
    SA_Fast             = 0x00020000,

    // Valid for: saFind
    // Only valid for sorted arrays, setting this flag will return
    // the index where the element would be inserted if there is no
    // exact match.
    SA_Inexact          = 0x00100000,

    // ----- INTERNAL USE ONLY -----
    // Destroys original element after inserting.
    SAINT_Consume       = 0x10000000,
};

#define SA_GROW_MASK (0xff000000)
#define SA_Grow(rate) (((uint32)SA_GROW_##rate) << 24)
#define SA_GET_GROW(flags) ((flags) >> 24)

#define SARRAY_HDRSIZE (offsetof(SArrayHeader, data))
#define SARRAY_HDR(ref) ((SArrayHeader*)(((uintptr)((ref).a)) - SARRAY_HDRSIZE))
#define SAREF(r) (unused_noeval(&((r)._is_sarray)), *(sa_ref*)(&(r)))
#define SAHANDLE(h) ((sahandle)(unused_noeval((h != NULL) && &((h)->_is_sarray)), (h)))

#define saSize(ref) ((ref)._is_sarray ? _saHdr(SAREF(ref))->count : 0)
#define saCapacity(ref) ((ref)._is_sarray ? _saHdr(SAREF(ref))->capacity : 0)
#define saElemSize(ref) ((ref)._is_sarray ? stGetSize(_saHdr(SAREF(ref))->elemtype) : 0)
#define saElemType(ref) ((ref)._is_sarray ? _saHdr(SAREF(ref))->elemtype : 0)
#define saValid(ref) ((ref).a)

_Ret_notnull_
_meta_inline SArrayHeader *_saHdr(_In_ sa_ref ref)
{
    return SARRAY_HDR(ref);
}

_Success_(!canfail || return)
_When_(canfail, _Check_return_)
_At_(out->a, _Post_notnull_)
bool _saInit(_Out_ sahandle out, stype elemtype, _In_opt_ STypeOps *ops, int32 capacity, bool canfail, flags_t flags);
#define saInit(out, type, capacity, ...) _saInit(SAHANDLE(out), stFullType(type), capacity, false, opt_flags(__VA_ARGS__))
#define saTryInit(out, type, capacity, ...) _saInit(SAHANDLE(out), stFullType(type), capacity, true, opt_flags(__VA_ARGS__))

_At_(handle->a, _Pre_maybenull_ _Post_null_)
void _saDestroy(_Inout_ sahandle handle);
#define saDestroy(handle) _saDestroy(SAHANDLE(handle))

_When_(canfail, _Check_return_)
_At_(handle->a, _Pre_notnull_ _Post_notnull_)
bool _saReserve(_Inout_ sahandle handle, int32 capacity, bool canfail);
#define saReserve(handle, capacity) _saReserve(SAHANDLE(handle), capacity, true)

_At_(handle->a, _Pre_notnull_ _Post_notnull_)
void _saShrink(_Inout_ sahandle handle, int32 capacity);
#define saShrink(handle, capacity) _saShrink(SAHANDLE(handle), capacity)

_At_(handle->a, _Pre_notnull_ _Post_notnull_)
void _saSetSize(_Inout_ sahandle handle, int32 size);
#define saSetSize(handle, size) _saSetSize(SAHANDLE(handle), size)

_At_(handle->a, _Pre_maybenull_)
void _saClear(_Inout_ sahandle handle);
#define saClear(handle) _saClear(SAHANDLE(handle))

#define _sa_Consume_Arg_ _When_(flags & SAINT_Consume, _Pre_notnull_ _Post_invalid_) _When_(!(flags & SAINT_Consume), _Inout_) 
_At_(handle->a, _Pre_maybenull_ _Post_notnull_)
int32 _saPush(_Inout_ sahandle handle, stype elemtype, _In_ stgeneric elem, flags_t flags);
_At_(handle->a, _Pre_maybenull_ _Post_notnull_)
int32 _saPushPtr(_Inout_ sahandle handle, stype elemtype, _sa_Consume_Arg_ stgeneric *elem, flags_t flags);
#define saPush(handle, type, elem, ...) _saPush(SAHANDLE(handle), stCheckedArg(type, elem), opt_flags(__VA_ARGS__))
// Consume version of push, requires that elem be a pointer
#define saPushC(handle, type, elem, ...) _saPushPtr(SAHANDLE(handle), stCheckedPtrArg(type, elem), opt_flags(__VA_ARGS__) | SAINT_Consume)

// Pointer pop transfers ownership to the caller and does not call the destructor
_Ret_opt_valid_
_At_(handle->a, _Pre_maybenull_)
void *_saPopPtr(_Inout_ sahandle handle, int32 idx);
#define saPopPtr(handle) _saPopPtr(SAHANDLE(handle), -1)
#define saPopPtrI(handle, idx) _saPopPtr(SAHANDLE(handle), idx)

_At_(ref.a, _Pre_notnull_)
int32 _saFind(_In_ sa_ref ref, _In_ stgeneric elem, flags_t flags);
_At_(ref.a, _Pre_maybenull_)
_meta_inline int32 _saFindChecked(_In_ sa_ref ref, stype elemtype, _In_ stgeneric elem, flags_t flags)
{
    if (!ref.a)
        return -1;
    devAssert(stEq(saElemType(ref), elemtype));
    return _saFind(ref, elem, flags);
}
#define saFind(ref, type, elem, ...) _saFindChecked(SAREF(ref), stCheckedArg(type, elem), opt_flags(__VA_ARGS__))

_At_(handle->a, _Pre_notnull_ _Post_notnull_)
bool _saFindRemove(_Inout_ sahandle handle, _In_ stgeneric elem, flags_t flags);
_At_(handle->a, _Pre_maybenull_)
_meta_inline bool _saFindRemoveChecked(_Inout_ sahandle handle, stype elemtype, _In_ stgeneric elem, flags_t flags)
{
    if (!handle->a)
        return false;
    devAssert(stEq(saElemType(*handle), elemtype));
    return _saFindRemove(handle, elem, flags);
}
#define saFindRemove(handle, type, elem, ...) _saFindRemoveChecked(SAHANDLE(handle), stCheckedArg(type, elem), opt_flags(__VA_ARGS__))

_At_(handle->a, _Pre_notnull_ _Post_notnull_)
int32 _saInsert(_Inout_ sahandle handle, int32 idx, _In_ stgeneric elem);
_At_(handle->a, _Pre_notnull_ _Post_notnull_)
_meta_inline int32 _saInsertChecked(_Inout_ sahandle handle, int32 idx, stype elemtype, _In_ stgeneric elem)
{
    devAssert(handle->_is_sarray);
    devAssert(stEq(saElemType(*handle), elemtype));
    return _saInsert(handle, idx, elem);
}
#define saInsert(handle, idx, type, elem) _saInsertChecked(SAHANDLE(handle), idx, stCheckedArg(type, elem))

_At_(handle->a, _Pre_notnull_ _Post_notnull_)
bool _saExtract(_Inout_ sahandle handle, int32 idx, _Inout_opt_ stgeneric *elem, flags_t flags);

_At_(handle->a, _Pre_maybenull_)
_meta_inline bool _saExtractChecked(_Inout_ sahandle handle, int32 idx, stype elemtype, _stCopyDest_Anno_opt_(elemtype) stgeneric *elem, flags_t flags)
{
    if (!handle->a)
        return false;
    devAssert(stGetId(elemtype) == stTypeId(none) || stEq(saElemType(*handle), elemtype));
    return _saExtract(handle, idx, elem, flags);
}
#define htExtract(htbl, ktype, key, vtype, val_copy_out) _htExtractChecked(htbl, stCheckedArg(ktype, key), stCheckedPtrArg(vtype, val_copy_out))
#define saExtract(handle, idx, type, elem_copy_out, ...) _saExtractChecked(SAHANDLE(handle), idx, stCheckedPtrArg(type, elem_copy_out), opt_flags(__VA_ARGS__))
#define saRemove(handle, idx, ...) _saExtractChecked(SAHANDLE(handle), idx, stType(none), NULL, opt_flags(__VA_ARGS__))

_At_(handle->a, _Pre_maybenull_)
void _saSort(_Inout_ sahandle handle, bool keep);
#define saSort(handle, keep) _saSort(SAHANDLE(handle), keep)

_At_(out->a, _When_(ref.a, _Post_notnull_))
void _saSlice(_Out_ sahandle out, _In_ sa_ref ref, int32 start, int32 end);
#define saSlice(out, src, start, end) _saSlice(SAHANDLE(out), SAREF(src), start, end)
#define saClone(out, src) _saSlice(SAHANDLE(out), SAREF(src), 0, 0)

_At_(out->a, _Post_maybenull_)
void _saMerge(_Out_ sahandle out, int n, _In_ sa_ref *refs, flags_t flags);
#define saMerge(out, ...) _saMerge(SAHANDLE(out), sizeof((sa_ref[]){ __VA_ARGS__ })/sizeof(sa_ref), (sa_ref[]){ __VA_ARGS__ }, 0)
#define saMergeF(out, flags, ...) _saMerge(SAHANDLE(out), sizeof((sa_ref[]){ __VA_ARGS__ })/sizeof(sa_ref), (sa_ref[]){ __VA_ARGS__ }, flags)
