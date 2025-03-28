#include "sarray_private.h"
#include "cx/utils.h"
#include "cx/string.h"

#if 0
// inline functions get emitted in this translation unit
// Needed as technically C99 doesn't guarantee inlining, but disabled for now
// as all of our supported compilers have a way to enforce a guarantee.
extern inline int32 _saPushChecked(sahandle handle, stype elemtype, void *elem, flags_t flags)
extern inline int32 _saFindChecked(sahandle handle, stype elemtype, void *elem, flags_t flags);
extern inline int32 _saInsertChecked(sahandle handle, int32 idx, stype elemtype, void *elem);
#endif

// qsort routine from Bentley & McIlroy's "Engineering a Sort Function"
static _meta_inline void
sa_swap(_Inout_updates_bytes_(sz) void *av, _Inout_updates_bytes_(sz) void *bv, size_t sz)
{
    uint8 *a = av;
    uint8 *b = bv;
    uint8 t;

    do {
        t = *a;
        *a++ = *b;
        *b++ = t;
    } while (--sz > 0);
}

#define vecswap(a, b, n) \
    if ((n) > 0) sa_swap(a, b, n)

// specializations for various data types for performance
#define SA_SPECIALIZE_FINDONLY(name, typ, comptyp, compfunc) \
static int32 sa_find_internal_##name(_In_ SArrayHeader *hdr, _In_ const void *elem, _Inout_ bool *found) \
{ \
    int32 i; \
\
    for (i = 0; i < hdr->count; ++i) { \
        if (compfunc(hdr->elemtype, elem, ELEMPTR(hdr, i)) == 0) { \
            *found = true; \
            return i; \
        } \
    } \
\
    return hdr->count; \
}

#define SA_SPECIALIZE_BSEARCHONLY(name, typ, comptyp, compfunc) \
static int32 sa_bsearch_internal_##name(_In_ SArrayHeader *hdr, _In_ const void *elem, _Inout_ bool *found) \
{ \
    int32 low, high, i; \
    comptyp comp; \
\
    low = 0; \
    high = hdr->count; \
    while(high > low) { \
        i = (high + low) / 2; \
        comp = compfunc(hdr->elemtype, elem, ELEMPTR(hdr, i)); \
        if (comp == 0) { \
            low = high = i; \
            *found = true; \
        } else if (comp < 0) \
            high = i; \
        else if (i == low) \
            low = high; \
        else \
            low = i; \
    } \
    return low; \
}

#define SA_SPECIALIZE_QSORTONLY(name, typ, comptyp, compfunc) \
static _meta_inline char * \
med3_##name(_In_ SArrayHeader *hdr, _In_ void *a, _In_ void *b, _In_ void *c) \
{ \
    return (char*)(compfunc(hdr->elemtype, a, b) < 0 ? \
        (compfunc(hdr->elemtype, b, c) < 0 ? b : (compfunc(hdr->elemtype, a, c) < 0 ? c : a )) \
       :(compfunc(hdr->elemtype, b, c) > 0 ? b : (compfunc(hdr->elemtype, a, c) < 0 ? a : c ))); \
} \
\
static void sa_qsort_internal_##name(_Inout_ SArrayHeader *hdr, _Inout_ void *a, size_t n) \
{ \
    char *pa, *pb, *pc, *pd, *pm, *pn; \
    size_t es, d1, d2; \
    comptyp cmp_result; \
    int swap_cnt; \
    \
    es = stGetSize(hdr->elemtype); \
\
loop: \
    swap_cnt = 0; \
\
    if (n < 7) { \
        for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es) \
            for (char *pl = pm; \
                 pl > (char *)a && compfunc(hdr->elemtype, (void*)(pl - es), (void*)pl) > 0; \
                 pl -= es) \
                sa_swap(pl, pl - es, es); \
        return; \
    } \
    pm = (char *)a + (n / 2) * es; \
    if (n > 7) { \
        char *pl = a; \
        pn = (char *)a + (n - 1) * es; \
        if (n > 40) { \
            size_t d = (n / 8) * es; \
\
            pl = med3_##name(hdr, pl, pl + d, pl + 2 * d); \
            pm = med3_##name(hdr, pm - d, pm, pm + d); \
            pn = med3_##name(hdr, pn - 2 * d, pn - d, pn); \
        } \
        pm = med3_##name(hdr, pl, pm, pn); \
    } \
    sa_swap(a, pm, es); \
    pa = pb = (char *)a + es; \
\
    pc = pd = (char *)a + (n - 1) * es; \
    for (;;) { \
        while (pb <= pc && (cmp_result = compfunc(hdr->elemtype, (void*)pb, a)) <= 0) { \
            if (cmp_result == 0) { \
                swap_cnt = 1; \
                sa_swap(pa, pb, es); \
                pa += es; \
            } \
            pb += es; \
        } \
        while (pb <= pc && (cmp_result = compfunc(hdr->elemtype, (void*)pc, a)) >= 0) { \
            if (cmp_result == 0) { \
                swap_cnt = 1; \
                sa_swap(pc, pd, es); \
                pd -= es; \
            } \
            pc -= es; \
        } \
        if (pb > pc) \
            break; \
        sa_swap(pb, pc, es); \
        swap_cnt = 1; \
        pb += es; \
        pc -= es; \
    } \
    if (swap_cnt == 0) {  /* Switch to insertion sort */ \
        for (pm = (char *)a + es; pm < (char *)a + n * es; pm += es) \
            for (char *pl = pm; \
                 pl > (char *)a && compfunc(hdr->elemtype, (void*)(pl - es), (void*)pl) > 0; \
                 pl -= es) \
                sa_swap(pl, pl - es, es); \
        return; \
    } \
\
    pn = (char *)a + n * es; \
    d1 = min((pa - (char *)a), pb - pa); \
    vecswap(a, pb - d1, d1); \
    d1 = min((size_t)(pd - pc), pn - pd - es); \
    vecswap(pb, pn - d1, d1); \
\
    d1 = pb - pa; \
    d2 = pd - pc; \
    if (d1 <= d2) { \
        /* Recurse on left partition, then iterate on right partition */ \
        if (d1 > es) { \
            sa_qsort_internal_##name(hdr, a, d1 / es); \
        } \
        if (d2 > es) { \
            /* Iterate rather than recurse to save stack space */ \
            /* qsort(pn - d2, d2 / es, es, cmp); */ \
            a = pn - d2; \
            n = d2 / es; \
            goto loop; \
        } \
    } else { \
        /* Recurse on right partition, then iterate on left partition */ \
        if (d2 > es) { \
            sa_qsort_internal_##name(hdr, pn - d2, d2 / es); \
        } \
        if (d1 > es) { \
            /* Iterate rather than recurse to save stack space */ \
            /* qsort(a, d1 / es, es, cmp); */ \
            n = d1 / es; \
            goto loop; \
        } \
    } \
}

#define SA_SPECIALIZE(name, typ, comptyp, compfunc) \
SA_SPECIALIZE_FINDONLY(name, typ, comptyp, compfunc) \
SA_SPECIALIZE_BSEARCHONLY(name, typ, comptyp, compfunc) \
SA_SPECIALIZE_QSORTONLY(name, typ, comptyp, compfunc)

#define compfunc_stype(st, a, b) _stCmp(st, NULL, stStored(st, a), stStored(st, b), 0)
SA_SPECIALIZE_BSEARCHONLY(stype, void*, intptr, compfunc_stype)
SA_SPECIALIZE_QSORTONLY(stype, void *, intptr, compfunc_stype)
#define compfunc_stype_eq(st, a, b) _stCmp(st, NULL, stStored(st, a), stStored(st, b), ST_Equality)
SA_SPECIALIZE_FINDONLY(stypeeq, void *, intptr, compfunc_stype_eq)
#define compfunc_stype_cmp(st, a, b) _stCmp(st, &hdr->typeops, stStored(st, a), stStored(st, b), 0)
SA_SPECIALIZE(stypecmp, void*, intptr, compfunc_stype_cmp)
#define compfunc_str(st, a, b) strCmp(*(string*)(a), *(string*)(b))
SA_SPECIALIZE(string, string, int32, compfunc_str)
#define SA_SPECIALIZE_BUILTIN(name, typ, comptyp) \
static _meta_inline comptyp compfunc_##name(stype st, const void *a, const void *b) { return *(typ*)a - *(typ*)b; } \
SA_SPECIALIZE(name, typ, comptyp, compfunc_##name)
SA_SPECIALIZE_BUILTIN(int8, int8, int)
SA_SPECIALIZE_BUILTIN(uint8, uint8, int)
SA_SPECIALIZE_BUILTIN(int16, int16, int)
SA_SPECIALIZE_BUILTIN(uint16, uint16, int)
SA_SPECIALIZE_BUILTIN(int32, int32, int)
SA_SPECIALIZE_BUILTIN(uint32, uint32, int)
SA_SPECIALIZE_BUILTIN(int64, int64, int64)
SA_SPECIALIZE_BUILTIN(uint64, uint64, int64)
SA_SPECIALIZE_BUILTIN(float32, float32, float32)
SA_SPECIALIZE_BUILTIN(float64, float64, float64)
SA_SPECIALIZE_BUILTIN(ptr, char*, intptr)

typedef int32(*sa_find_spec)(_In_ SArrayHeader *hdr, _In_ const void *elem, _Out_ bool *found);
typedef void(*sa_qsort_spec)(_Inout_ SArrayHeader *hdr, _Inout_ void *a, size_t n);

static LazyInitState spec_init_state;
static sa_find_spec *find_spec;
static sa_find_spec *bsearch_spec;
static sa_qsort_spec *qsort_spec;

#define SA_SPEC_INIT_ONE(type) \
    find_spec[stTypeId(type)] = sa_find_internal_##type; \
    bsearch_spec[stTypeId(type)] = sa_bsearch_internal_##type; \
    qsort_spec[stTypeId(type)] = sa_qsort_internal_##type;

static void sa_spec_init(void *user)
{
    find_spec = xaAlloc(256 * sizeof(sa_find_spec), XA_Zero);
    bsearch_spec = xaAlloc(256 * sizeof(sa_find_spec), XA_Zero);
    qsort_spec = xaAlloc(256 * sizeof(sa_qsort_spec), XA_Zero);

    SA_SPEC_INIT_ONE(string);
    SA_SPEC_INIT_ONE(int8);
    SA_SPEC_INIT_ONE(int16);
    SA_SPEC_INIT_ONE(int32);
    SA_SPEC_INIT_ONE(int64);
    SA_SPEC_INIT_ONE(uint8);
    SA_SPEC_INIT_ONE(uint16);
    SA_SPEC_INIT_ONE(uint32);
    SA_SPEC_INIT_ONE(uint64);
    SA_SPEC_INIT_ONE(float32);
    SA_SPEC_INIT_ONE(float64);
    SA_SPEC_INIT_ONE(ptr);
}

#define SA_FIND_DISPATCH_CASE(func, caseval, name) \
case caseval: \
    return func##_##name(hdr, elem, found);

static int32 sa_find_internal(_In_ SArrayHeader *hdr, _In_ stgeneric stelem, _Out_ bool *found)
{
    uint8 type = stGetId(hdr->elemtype);
    lazyInit(&spec_init_state, sa_spec_init, NULL);
    void *elem = stGenPtr(hdr->elemtype, stelem);

    *found = false;
    if (hdr->flags & SA_Sorted) {
        if ((hdr->flags & SAINT_Extended) && hdr->typeops.cmp)
            return sa_bsearch_internal_stypecmp(hdr, elem, found);

        if (bsearch_spec[type])
            return bsearch_spec[type](hdr, elem, found);
        return sa_bsearch_internal_stype(hdr, elem, found);
    } else {
        if ((hdr->flags & SAINT_Extended) && hdr->typeops.cmp)
            return sa_find_internal_stypecmp(hdr, elem, found);

        if (find_spec[type])
            return find_spec[type](hdr, elem, found);
        return sa_find_internal_stypeeq(hdr, elem, found);
    }

    // unreachable
}

static void sa_qsort_internal(_Inout_ SArrayHeader *hdr)
{
    uint8 type = stGetId(hdr->elemtype);
    lazyInit(&spec_init_state, sa_spec_init, NULL);

    if ((hdr->flags & SAINT_Extended) && hdr->typeops.cmp)
        sa_qsort_internal_stypecmp(hdr, hdr->data, hdr->count);
    else if (qsort_spec[type])
        qsort_spec[type](hdr, hdr->data, hdr->count);
    else
        sa_qsort_internal_stype(hdr, hdr->data, hdr->count);
}

_Use_decl_annotations_
bool _saInit(sahandle out, stype elemtype, STypeOps *ops, int32 capacity, bool canfail, flags_t flags)
{
    SArrayHeader *hdr;

    if (capacity == 0)
        capacity = 8;
    else
        capacity = max(capacity, 1);
    relAssert(stGetSize(elemtype) > 0);

    // If we are not using custom ops and this is a POD type, skip all of the copy/destructor
    // logic and just use straight memory copies for speed.
    if (!ops && !stHasFlag(elemtype, Object))
        flags |= SA_Ref;

    if (ops) {
        // need the full header
        flags |= SAINT_Extended;
        hdr = xaAlloc(SARRAY_HDRSIZE + (size_t)capacity * stGetSize(elemtype),
                      canfail ? XA_Optional(High) : 0);
        if (canfail && !hdr)
            return false;

        hdr->typeops = *ops;
    } else {
        // use the smaller header that saves a few bytes for each array
        // yes, this is evil
        // hdr technically points to unallocated memory, but we're careful to not touch the first part
        void *hbase = xaAlloc((SARRAY_HDRSIZE + (size_t)capacity * stGetSize(elemtype)) - SARRAY_SMALLHDR_OFFSET,
                              canfail ? XA_Optional(High) : 0);
        if (canfail && !hbase)
            return false;

        hdr = (SArrayHeader*)((uintptr_t)hbase - SARRAY_SMALLHDR_OFFSET);
    }

    if (SA_GET_GROW(flags) == SA_GROW_Auto) {
        // auto growth based on element size
        if (stGetSize(elemtype) <= 8)
            flags |= SA_Grow(Aggressive);
        else if (stGetSize(elemtype) <= 256)
            flags |= SA_Grow(Normal);
        else
            flags |= SA_Grow(Slow);
    }

    hdr->elemtype = elemtype;
    hdr->count = 0;
    hdr->capacity = capacity;
    hdr->flags = flags;
    out->a = &hdr->data[0];

    return true;
}

static bool saRealloc(_Inout_ sahandle handle, _Inout_ptr_ SArrayHeader **hdr, int32 cap, bool canfail)
{
    bool ret = false;
    if ((*hdr)->flags & SAINT_Extended) {
        ret = xaResize(hdr, SARRAY_HDRSIZE + (size_t)cap * stGetSize((*hdr)->elemtype), canfail ? XA_Optional(High) : 0);
    } else {
        // ugly non-extended header
        void *smlbase = (void*)((uintptr_t)(*hdr) + SARRAY_SMALLHDR_OFFSET);
        ret = xaResize(&smlbase, (SARRAY_HDRSIZE + (size_t)cap * stGetSize((*hdr)->elemtype)) - SARRAY_SMALLHDR_OFFSET, canfail ? XA_Optional(High) : 0);
        if (ret)
            *hdr = (SArrayHeader*)((uintptr_t)smlbase - SARRAY_SMALLHDR_OFFSET);
    }

    if (ret) {
        (*hdr)->capacity = cap;
        handle->a = &(*hdr)->data[0];
    }
    return ret;
}

static bool _saGrow(_Inout_ sahandle handle, _Inout_ptr_ SArrayHeader **hdr, int32 minsz, bool canfail)
{
    int32 cap = (*hdr)->capacity;

    while (cap < minsz) {
        switch (SA_GET_GROW((*hdr)->flags)) {
        case SA_GROW_Normal:
            if (cap < 16)
                cap *= 2;           // 100%
            else if (cap < 128)
                cap += (cap >> 1);  // 50%
            else
                cap += (cap >> 2);  // 25%
            break;
        case SA_GROW_Aggressive:
            if (cap < 32)
                cap *= 2;           // 100%
            else if (cap < 256)
                cap += (cap >> 1);  // 50%
            else
                cap += (cap >> 2);  // 25%
            break;
        case SA_GROW_Slow:
            if (cap < 8)
                cap *= 2;           // 100%
            else if (cap < 64)
                cap += (cap >> 1);  // 50%
            else
                cap += (cap >> 2);  // 25%
            break;
        case SA_GROW_100:
            cap *= 2;               // 100%
            break;
        case SA_GROW_50:
            cap += (cap >> 1);      // 50%
            break;
        case SA_GROW_25:
            cap += (cap >> 2);      // 25%
            break;
        case SA_GROW_Minimal:
            cap = max(cap, minsz);
            break;
        default:
            devAssert("Invalid sarray grow setting");
            cap += (cap >> 1);
        }
    }

    return saRealloc(handle, hdr, cap, canfail);
}

_Use_decl_annotations_
void _saDestroy(sahandle handle)
{
    if (!handle->a)
        return;

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    _saClear(handle);           // to call dtors
    if (hdr->flags & SAINT_Extended) {
        xaFree(hdr);
    } else {
        void *smbase = (void*)((uintptr_t)hdr + SARRAY_SMALLHDR_OFFSET);
        xaFree(smbase);
    }
    handle->a = NULL;
}

_Use_decl_annotations_
void _saClear(sahandle handle)
{
    if (!handle->a)
        return;

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    if (!(hdr->flags & SA_Ref)) {
        STypeOps *ops = HDRTYPEOPS(hdr);
        int32 i;

        for (i = 0; i < hdr->count; ++i) {
            _stDestroy(hdr->elemtype, ops,
                       stStoredPtr(hdr->elemtype, ELEMPTR(hdr, i)), 0);
        }
    }
    hdr->count = 0;
}

_Use_decl_annotations_
bool _saReserve(sahandle handle, int32 capacity, bool canfail)
{
    if (!handle->a)
        return false;

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    int32 newcap = max((capacity == 0) ? 1 : capacity, hdr->count);
    if (newcap > hdr->capacity) {
        return _saGrow(handle, &hdr, newcap, canfail);
    }
    return true;
}

_Use_decl_annotations_
void _saShrink(sahandle handle, int32 capacity)
{
    if (!handle->a)
        return;

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    int32 newcap = (capacity == 0) ? 1 : capacity;
    if (newcap < hdr->capacity) {
        saRealloc(handle, &hdr, newcap, true);
        if (capacity < hdr->count)
            hdr->count = capacity;
    }
}

_Use_decl_annotations_
void _saSetSize(sahandle handle, int32 size)
{
    if (!handle->a)
        return;

    _saReserve(handle, size, false);
    SArrayHeader *hdr = SARRAY_HDR(*handle);
    STypeOps *ops = HDRTYPEOPS(hdr);

    if (size > hdr->count) {
        memset(ELEMPTR(hdr, hdr->count), 0, ((uintptr)size - hdr->count) * stGetSize(hdr->elemtype));
    } else if (!(hdr->flags & SA_Ref)) {
        for (int i = size; i < hdr->count; i++) {
            _stDestroy(hdr->elemtype, ops,
                       stStoredPtr(hdr->elemtype, ELEMPTR(hdr, i)), 0);
        }
    }
    hdr->count = size;
}

static _meta_inline void sa_set_elem_internal(_Inout_ SArrayHeader *hdr, int32 idx, _When_(!consume, _In_) _When_(consume, _Pre_notnull_ _Post_invalid_) stgeneric *elem, bool consume)
{
    if (consume) {
        // special case: if we're consuming, just steal the element instead of deep copying it,
        // even if we're the owner
        memcpy(ELEMPTR(hdr, idx), stGenPtr(hdr->elemtype, *elem), stGetSize(hdr->elemtype));

        // destroy source
        if (hdr->flags & SA_Ref)        // weird combo, but respect it
            _stDestroy(hdr->elemtype, HDRTYPEOPS(hdr), elem, 0);
        else if (stGetSize(hdr->elemtype) == sizeof(void*))
            elem->st_ptr = 0;           // if this is a pointer-sized element, clear it out
        return;
    }

    if (!(hdr->flags & SA_Ref))
        _stCopy(hdr->elemtype, HDRTYPEOPS(hdr),
                stStoredPtr(hdr->elemtype, ELEMPTR(hdr, idx)), *elem, 0);
    else
        memcpy(ELEMPTR(hdr, idx), stGenPtr(hdr->elemtype, *elem), stGetSize(hdr->elemtype));
}

static int32 sa_insert_internal(_Inout_ sahandle handle, _Inout_ SArrayHeader *hdr, int32 idx, _When_(!consume, _In_) _When_(consume, _Pre_notnull_ _Post_invalid_) stgeneric *elem, bool consume)
{
    if (hdr->count == hdr->capacity)
        _saGrow(handle, &hdr, hdr->count + 1, false);

    memmove(ELEMPTR(hdr, (size_t)idx + 1), ELEMPTR(hdr, idx), ((size_t)hdr->count - idx) * stGetSize(hdr->elemtype));
    hdr->count++;

    sa_set_elem_internal(hdr, idx, elem, consume);

    return idx;
}

_Use_decl_annotations_
int32 _saInsert(sahandle handle, int32 idx, stgeneric elem)
{
    SArrayHeader *hdr = SARRAY_HDR(*handle);

    // negative indicies represent distance from the end of the array
    if (idx < 0)
        idx += hdr->count;

    if (idx < 0 || idx > hdr->count)
        return -1;

    // inserting into a sorted array makes it not be sorted anymore
    if ((hdr->flags & SA_Sorted))
        hdr->flags &= ~SA_Sorted;

    return sa_insert_internal(handle, hdr, idx, &elem, false);
}

_Use_decl_annotations_
int32 _saPushPtr(sahandle handle, stype elemtype, stgeneric *elem, flags_t flags)
{
    if (!handle->a)
        _saInit(handle, elemtype, NULL, 0, false, 0);
    devAssert(stEq(saElemType(*handle), elemtype));

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    STypeOps *ops = HDRTYPEOPS(hdr);

    if (hdr->flags & SA_Sorted) {
        bool found = false;
        int32 idx = sa_find_internal(hdr, *elem, &found);
        if (found && (flags & SA_Unique)) {
            if (flags & SAINT_Consume)
                _stDestroy(hdr->elemtype, ops, elem, 0);
            return -1;          // don't insert if we already have it
        }

        idx = sa_insert_internal(handle, hdr, idx, elem, flags & SAINT_Consume);
        return idx;
    } else {
        if (flags & SA_Unique) {
            bool found = false;
            sa_find_internal(hdr, *elem, &found);
            if (found) {
                if (flags & SAINT_Consume)
                    _stDestroy(hdr->elemtype, ops, elem, 0);
                return -1;
            }
        }

        if (hdr->count == hdr->capacity)
            _saGrow(handle, &hdr, hdr->count + 1, false);

        sa_set_elem_internal(hdr, hdr->count, elem, flags & SAINT_Consume);

        hdr->count++;
        return hdr->count - 1;
    }
}

_Use_decl_annotations_
int32 _saPush(sahandle handle, stype elemtype, stgeneric elem, flags_t flags)
{
    return _saPushPtr(handle, elemtype, &elem, flags);
}

static void sa_remove_internal(_Inout_ sahandle handle, _Inout_ SArrayHeader *hdr, int32 idx, bool fast)
{
    if (idx != hdr->count - 1) {
        if (fast) {
            // swap last element with the one being removed -- this changes the order!
            if ((hdr->flags & SA_Sorted))
                hdr->flags &= ~SA_Sorted;
            memcpy(ELEMPTR(hdr, idx),
                   ELEMPTR(hdr, (size_t)hdr->count - 1),
                   stGetSize(hdr->elemtype));
        } else {
            memmove(ELEMPTR(hdr, idx),
                    ELEMPTR(hdr, (size_t)idx + 1),
                    ((size_t)hdr->count - idx - 1) * stGetSize(hdr->elemtype));
        }
    }

    hdr->count--;
}

_Use_decl_annotations_
bool _saExtract(sahandle handle, int32 idx, stgeneric *elem, flags_t flags)
{
    if (!handle->a)
        return false;

    SArrayHeader *hdr = SARRAY_HDR(*handle);

    if (idx < 0)
        idx += hdr->count;

    if (idx < 0 || idx >= hdr->count)
        return false;

    // either extract into a variable if one is provided, or just destroy it
    if (elem)
        memcpy(stGenPtr(hdr->elemtype, *elem), ELEMPTR(hdr, idx), stGetSize(hdr->elemtype));
    else if (!(hdr->flags & SA_Ref))
        _stDestroy(hdr->elemtype, HDRTYPEOPS(hdr),
                   stStoredPtr(hdr->elemtype, ELEMPTR(hdr, idx)), 0);

    sa_remove_internal(handle, hdr, idx, flags & SA_Fast);

    if (hdr->flags & SA_AutoShrink) {
        saRealloc(handle, &hdr, hdr->count, true);
    }
    return true;
}

_Use_decl_annotations_
void *_saPopPtr(sahandle handle, int32 idx)
{
    if (!handle->a)
        return NULL;

    // Destructor is intentionally not called, as we are
    // giving our reference to the caller.

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    devAssert(stGetId(hdr->elemtype) == stTypeId(ptr) ||
              stGetId(hdr->elemtype) == stTypeId(object));

    if (hdr->count == 0)
        return NULL;

    if (idx == -1) {
        // special case for popping from the tail
        hdr->count--;
        return *(void**)ELEMPTR(hdr, hdr->count);
    }

    if (idx < 0)
        idx += hdr->count;

    if (idx < 0 || idx >= hdr->count)
        return NULL;

    void *ret = *(void**)ELEMPTR(hdr, idx);
    sa_remove_internal(handle, hdr, idx, false);
    return ret;
}

_Use_decl_annotations_
int32 _saFind(sa_ref ref, stgeneric elem, flags_t flags)
{
    SArrayHeader *hdr = SARRAY_HDR(ref);

    int32 idx = -1;
    bool found = false;

    idx = sa_find_internal(hdr, elem, &found);

    if (!found && !((flags & SA_Inexact) && (hdr->flags & SA_Sorted)))
        return -1;

    return idx;
}

_Use_decl_annotations_
bool _saFindRemove(sahandle handle, stgeneric elem, flags_t flags)
{
    SArrayHeader *hdr = SARRAY_HDR(*handle);

    int32 idx = -1;
    bool found = false;

    idx = sa_find_internal(hdr, elem, &found);

    if (found)
        _saExtract(handle, idx, NULL, flags);

    return found;
}

_Use_decl_annotations_
void _saSort(sahandle handle, bool keep)
{
    if (!handle->a)
        return;

    SArrayHeader *hdr = SARRAY_HDR(*handle);
    sa_qsort_internal(hdr);

    if (keep)
        hdr->flags |= SA_Sorted;
}

_Use_decl_annotations_
void _saSlice(sahandle out, sa_ref ref, int32 start, int32 end)
{
    if (!ref.a) {
        out->a = NULL;
        return;
    }

    SArrayHeader *hdr = SARRAY_HDR(ref);
    stype elemtype = hdr->elemtype;

    // negative start indicates distance from end of array
    if (start < 0)
        start = clamphigh(clamplow(start + hdr->count, 0), hdr->count);
    else
        start = clamphigh(start, hdr->count);

    // negative end can indicate that as well
    if (end < 0)
        end = clamphigh(clamplow(end + hdr->count, 0), hdr->count);
    else if (end == 0)
        end = hdr->count;       // 0 end means rest of array
    else
        end = clamphigh(end, hdr->count);

    _saInit(out, elemtype, HDRTYPEOPS(hdr), clamplow(end - start, 1), false, hdr->flags);
    SArrayHeader *newhdr = SARRAY_HDR(*out);

    // mask out sorted flag for now to speed up inserts
    newhdr->flags &= ~SA_Sorted;

    int32 i;
    for (i = start; i < end; i++) {
        _saPushPtr(out, elemtype, stStoredPtr(hdr->elemtype, ELEMPTR(hdr, i)), 0);
    }

    // restore flags
    newhdr->flags = hdr->flags;
}

_Use_decl_annotations_
void _saMerge(sahandle out, int n, sa_ref *refs, flags_t flags)
{
    int32 newsize = 0;

    out->a = NULL;

    if (n == 0)
        return;

    // merged array gets flags from first array
    SArrayHeader *fhdr = SARRAY_HDR(refs[0]);
    stype elemtype = fhdr->elemtype;

    for (int i = 0; i < n; i++) {
        SArrayHeader *shdr = SARRAY_HDR(refs[i]);
        devAssert(stEq(fhdr->elemtype, shdr->elemtype));
        newsize += shdr->count;
    }

    _saInit(out, fhdr->elemtype, HDRTYPEOPS(fhdr), newsize, false, fhdr->flags);

    for (int i = 0; i < n; i++) {
        SArrayHeader *shdr = SARRAY_HDR(refs[i]);
        for (int32 j = 0; j < shdr->count; j++) {
            _saPushPtr(out, elemtype, stStoredPtr(shdr->elemtype, ELEMPTR(shdr, j)), flags);
        }
    }
}
