#pragma once

#include <cx/cx.h>
#include <cx/debug/assert.h>

typedef struct hashtable_ref {
    void *_is_hashtable;
} hashtable_ref;

typedef struct hashtable_ref *hashtable;
typedef struct HTChunkInfo HTChunkInfo;

typedef struct HashTableHeader {
    // extended header, present only if HT_Extended is set
    STypeOps keytypeops;
    STypeOps valtypeops;

    // hashtable header begins here
    uint32 idxsz;               // index size
    uint32 idxused;             // number of used index entries (including deleted)
    uint32 storsz;              // number of chunks in storage array
    uint32 storused;            // number of used storage slots (including deleted)
    uint32 valid;               // number of valid items in the hash table

    stype keytype;
    stype valtype;
    uint32 flags;

    HTChunkInfo *chunks;
    void **keystorage;
    void **valstorage;
    uint32 index[1];
} HashTableHeader;

// htelem is just the slot number (0 is reserved for "null")
typedef uint32 htelem;

typedef struct htiter {
    HashTableHeader *hdr;
    uint32 slot;
} htiter;

// HT_SLOTS_PER_CHUNK MUST BE A POWER OF TWO!
#ifdef _64BIT
#define HT_CHUNK_SHIFT 6
#else
#define HT_CHUNK_SHIFT 5
#endif
#define HT_SLOTS_PER_CHUNK (1 << HT_CHUNK_SHIFT)
#define HT_CHUNK_MASK (HT_SLOTS_PER_CHUNK - 1)
#define HT_QUARTER_CHUNK (HT_SLOTS_PER_CHUNK >> 2)
#define HT_QCHUNK_MASK (HT_QUARTER_CHUNK - 1)

#define HTABLE_HDRSIZE (offsetof(HashTableHeader, index))
#define HTABLE_HDR(ref) ((HashTableHeader*)(((uintptr)(&((ref)->_is_hashtable))) - HTABLE_HDRSIZE))

_Ret_valid_
_meta_inline HashTableHeader *_htHdr(_In_ hashtable ref)
{
    return HTABLE_HDR(ref);
}

#define htSize(ref) ((ref) ? _htHdr((ref))->valid : 0)
#define htKeyType(ref) ((ref) ? _htHdr((ref))->keytype : 0)
#define htValType(ref) ((ref) ? _htHdr((ref))->valtype : 0)
#define hteKeyPtrHdr(hdr, type, elem) ((stStorageType(type)*)_hteElemKeyPtr(hdr, elem))
#define hteValPtrHdr(hdr, type, elem) ((stStorageType(type)*)_hteElemValPtr(hdr, elem))
#define hteKeyPtr(ref, type, elem) hteKeyPtrHdr(_htHdr(ref), type, elem)
#define hteValPtr(ref, type, elem) hteValPtrHdr(_htHdr(ref), type, elem)
#define hteKey(ref, type, elem) (*hteKeyPtr(ref, type, elem))
#define hteVal(ref, type, elem) (*hteValPtr(ref, type, elem))
#define htiKeyPtr(type, iter) (hteKeyPtrHdr((iter).hdr, type, (iter).slot))
#define htiValPtr(type, iter) (hteValPtrHdr((iter).hdr, type, (iter).slot))
#define htiKey(type, iter) (*htiKeyPtr(type, iter))
#define htiVal(type, iter) (*htiValPtr(type, iter))

enum HASHTABLE_FLAGS_ENUM {
    HT_CaseInsensitive = 0x0001,    // only for string keys
    HT_RefKeys         = 0x0002,    // use a borrowed reference for keys rather than copying them
    HT_Ref             = 0x0004,    // use a borrowed reference for values rather than copying them

    // By default the storage array grows by quarter-chunks to avoid wasting too much memory. High
    // traffic hash tables can set this flag to always allocate full chunks for better insert performance.
    HT_InsertOpt       = 0x0008,

    // Ultra-compact mode does not preallocate any storage memory. Inserts will be slow!
    // This is useful for tables that should be as small as possible and are used for read-mostly lookups.
    HT_Compact         = 0x0010,

    // internal use only, do not set manually
    HTINT_Quadratic    = 0x2000,    // use quadratic probing rather than linear
    HTINT_Pow2         = 0x4000,    // size forced to power of 2 to avoid division
    HTINT_Extended     = 0x8000,    // extended header is present
};

enum HASHTABLE_GROWBY_ENUM {
    HT_GROW_By100   = 0x00,     // Grow by 100% (balanced)
    HT_GROW_By200   = 0x10,     // Grow by 200%
    HT_GROW_By300   = 0x20,     // Grow by 300% (better performance)
    HT_GROW_By50    = 0x40,     // Grow by 50% (worse performance, memory efficient)
    HT_GROW_BY_MASK = 0xf0,
};

enum HASHTABLE_GROW_ENUM {
    HT_GROW_At75    = 0x00,     // Grow at 75% full (balanced)
    HT_GROW_At50    = 0x01,     // Grow at 50% full (better performance)
    HT_GROW_At90    = 0x02,     // Grow at 90% full (worse performance, memory efficient)
    HT_GROW_AT_MASK = 0x0f,

    // some presets
    HT_GROW_MaxSpeed = HT_GROW_At50 | HT_GROW_By300,
    HT_GROW_MinSize  = HT_GROW_At90 | HT_GROW_By50,
};

enum HASHTABLE_FUNC_FLAGS_ENUM {
    // Valid for: htInsert
    // Does not insert if a matching key already exists.
    HT_Ignore     = 0x00010000,

    // Valid for: htFind
    // If copying out an object-type variable, copy a borrowed reference rather
    // than acquiring a reference or making a copy.
    HT_Borrow     = 0x00020000,

    // internal use only
    HTINT_Consume = 0x10000000,
};

#define HT_GROW_MASK (0xff000000)
#define HT_Grow(flag) (((uint32)HT_GROW_##flag) << 24)
#define HT_GET_GROW(flags) ((flags) >> 24)

void _htInit(_Outptr_ hashtable *out, stype keytype, _In_opt_ STypeOps *keyops, stype valtype, _In_opt_ STypeOps *valops, uint32 initsz, flags_t flags);
#define htInit(out, keytype, valtype, initsz, ...) _htInit(out, stFullType(keytype), stFullType(valtype), initsz, opt_flags(__VA_ARGS__))

void htDestroy(_Inout_ptr_uninit_ hashtable *htbl);
void htClear(_Inout_ptr_ hashtable *htbl);
void htReindex(_Inout_ptr_ hashtable *htbl, uint32 minsz);
void htRepack(_Inout_ptr_ hashtable *htbl);
void htClone(_Outptr_ hashtable *out, _In_ hashtable ref);

htelem _htInsert(_Inout_ptr_ hashtable *htbl, _In_ stgeneric key, _In_ stgeneric val, flags_t flags);
_meta_inline htelem _htInsertChecked(_Inout_ptr_ hashtable *htbl, stype keytype, _In_ stgeneric key, stype valtype, _In_ stgeneric val, flags_t flags)
{
    devAssert(*htbl);
    devAssert(stEq(htKeyType(*htbl), keytype) && stEq(htValType(*htbl), valtype));
    return _htInsert(htbl, key, val, flags);
}
#define _ht_Consume_Arg_ _When_(flags & HTINT_Consume, _Pre_notnull_ _Post_invalid_) _When_(!(flags & HTINT_Consume), _Inout_) 
htelem _htInsertPtr(_Inout_ptr_ hashtable *htbl, _In_ stgeneric key, _ht_Consume_Arg_ stgeneric *val, flags_t flags);
_meta_inline htelem _htInsertCheckedC(_Inout_ptr_ hashtable *htbl, stype keytype, _In_ stgeneric key, stype valtype, _ht_Consume_Arg_ stgeneric *val, flags_t flags)
{
    devAssert(*htbl);
    devAssert(stEq(htKeyType(*htbl), keytype) && stEq(htValType(*htbl), valtype));
    return _htInsertPtr(htbl, key, val, flags);
}
#define htInsert(htbl, ktype, key, vtype, val, ...) _htInsertChecked(htbl, stCheckedArg(ktype, key), stCheckedArg(vtype, val), opt_flags(__VA_ARGS__))
// Consumes *value*, not key
#define htInsertC(htbl, ktype, key, vtype, val, ...) _htInsertCheckedC(htbl, stCheckedArg(ktype, key), stCheckedPtrArg(vtype, val), opt_flags(__VA_ARGS__) | HTINT_Consume)

_Success_(return != 0)
htelem _htFind(_In_ hashtable htbl, _In_ stgeneric key, _Inout_opt_ stgeneric *val, flags_t flags);
_Success_(return != 0)
_meta_inline htelem _htFindChecked(_In_ hashtable htbl, stype keytype, _In_ stgeneric key, stype valtype, _stCopyDest_Anno_opt_(valtype) stgeneric *val, flags_t flags)
{
    devAssert(htbl);
    devAssert(stEq(htKeyType(htbl), keytype));
    devAssert(stGetId(valtype) == stTypeId(none) || stEq(htValType(htbl), valtype));
    return _htFind(htbl, key, val, flags);
}
#define htFind(htbl, ktype, key, vtype, val_copy_out, ...) _htFindChecked(htbl, stCheckedArg(ktype, key), stCheckedPtrArg(vtype, val_copy_out), opt_flags(__VA_ARGS__))

// If val_copy_out is provided, the value is extracted into it rather than being destroyed
_Success_(return)
bool _htExtract(_Inout_ptr_ hashtable *htbl, _In_ stgeneric key, _Inout_opt_ stgeneric *val);
_Success_(return)
_meta_inline bool _htExtractChecked(_Inout_ptr_ hashtable *htbl, stype keytype, _In_ stgeneric key, stype valtype, _stCopyDest_Anno_opt_(valtype) stgeneric *val)
{
    devAssert(*htbl);
    devAssert(stEq(htKeyType(*htbl), keytype));
    devAssert(stGetId(valtype) == stTypeId(none) || stEq(htValType(*htbl), valtype));
    return _htExtract(htbl, key, val);
}
#define htExtract(htbl, ktype, key, vtype, val_copy_out) _htExtractChecked(htbl, stCheckedArg(ktype, key), stCheckedPtrArg(vtype, val_copy_out))
#define htRemove(htbl, ktype, key) _htExtractChecked(htbl, stCheckedArg(ktype, key), stType(none), NULL)

// Just checks for the existence of a key
bool _htHasKey(_In_ hashtable htbl, _In_ stgeneric key);
_meta_inline bool _htHasKeyChecked(_In_ hashtable htbl, stype keytype, _In_ stgeneric key)
{
    devAssert(htbl);
    devAssert(stEq(htKeyType(htbl), keytype));
    return _htHasKey(htbl, key);
}
#define htHasKey(htbl, ktype, key) _htHasKeyChecked(htbl, stCheckedArg(ktype, key))

_Pre_satisfies_(elem > 0)
void *_hteElemKeyPtr(_Inout_ HashTableHeader *hdr, htelem elem);
_Pre_satisfies_(elem > 0)
void *_hteElemValPtr(_Inout_ HashTableHeader *hdr, htelem elem);

// Hash table iterator
_Success_(return)
_Post_satisfies_(iter->slot > 0)
_On_failure_(_Post_satisfies_(iter->slot == 0))
bool htiInit(_Out_ htiter *iter, _In_ hashtable htbl);

_Success_(return)
_Post_satisfies_(iter->slot > 0)
_On_failure_(_Post_satisfies_(iter->slot == 0))
bool htiNext(_Inout_ htiter *iter);

void htiFinish(_Pre_notnull_ _Post_invalid_ htiter *iter);

_Post_equal_to_(iter->slot > 0)
_meta_inline bool htiValid(_In_ htiter *iter) { return iter->slot > 0; }
