#include "hashtable_private.h"
#include "cx/debug/assert.h"

#define _ht_Consume_Arg_Opt_ _When_(flags & HTINT_Consume, _Pre_notnull_ _Post_invalid_) _When_(!(flags & HTINT_Consume), _In_opt_) 
static uint32 _htInsertInternal(_Inout_ptr_ hashtable *htbl, _In_ stgeneric key, _ht_Consume_Arg_Opt_ stgeneric *val, flags_t flags);

static uint32 npow2(uint32 val)
{
    uint32 i;

    for (i = 0; i < 32; i++) {
        if (((uint32)1 << i) >= val)
            return 1 << i;
    }
    return 16;
}

static _meta_inline uint32 calcGrowth(_In_ HashTableHeader *hdr, uint32 sz)
{
    switch (HT_GET_GROW(hdr->flags) & HT_GROW_BY_MASK) {
    case HT_GROW_By100:
        return sz << 1;
    case HT_GROW_By200:
        return sz * 3;
    case HT_GROW_By300:
        return sz << 2;
    case HT_GROW_By50:
        return sz + (sz >> 1);
    default:
        devFatalError("Invalid hashtable grow amount");
        return sz << 1;
    }
}

static void _htNewChunk(_Inout_ HashTableHeader *hdr)
{
    // this should only ever be called when we're at a chunk boundary
    devAssert((hdr->storused & HT_CHUNK_MASK) == 0);
    uint32 chunk = HT_SLOT_CHUNK(hdr->storused);

    if (hdr->chunks) {
        // grow chunk array
        if (chunk >= hdr->storsz) {
            hdr->storsz = calcGrowth(hdr, hdr->storsz);
            xaResize(&hdr->chunks, hdr->storsz * sizeof(HTChunkInfo));
            xaResize(&hdr->keystorage, hdr->storsz * sizeof(void *));
            if (stGetSize(hdr->valtype) > 0)
                xaResize(&hdr->valstorage, hdr->storsz * sizeof(void *));
        }
    } else {
        hdr->chunks = xaAlloc(4 * sizeof(HTChunkInfo));
        hdr->keystorage = xaAlloc(4 * sizeof(void *));
        if (stGetSize(hdr->valtype) > 0)
            hdr->valstorage = xaAlloc(4 * sizeof(void *));
        hdr->storsz = 4;
    }

    HTChunkInfo *chunkinfo = &hdr->chunks[chunk];
    if (!(hdr->flags & (HT_InsertOpt | HT_Compact))) {
        // default chunk allocation strategy
        // allocate a quarter-chunk
        hdr->keystorage[chunk] = xaAlloc((size_t)HT_QUARTER_CHUNK * stGetSize(hdr->keytype));
        if (stGetSize(hdr->valtype) > 0)
            hdr->valstorage[chunk] = xaAlloc((size_t)HT_QUARTER_CHUNK * stGetSize(hdr->valtype));
        chunkinfo->nalloc = HT_QUARTER_CHUNK;
    } else if (hdr->flags & HT_InsertOpt) {
        // insert-optimized case, allocate a whole chunk
        hdr->keystorage[chunk] = xaAlloc((size_t)HT_SLOTS_PER_CHUNK * stGetSize(hdr->keytype));
        if (stGetSize(hdr->valtype) > 0)
            hdr->valstorage[chunk] = xaAlloc((size_t)HT_SLOTS_PER_CHUNK * stGetSize(hdr->valtype));
        chunkinfo->nalloc = HT_SLOTS_PER_CHUNK;
    } else { // if (hdr->flags & HT_Compact)
        // compact mode, allocate only the first slot
        hdr->keystorage[chunk] = xaAlloc(stGetSize(hdr->keytype));
        if (stGetSize(hdr->valtype) > 0)
            hdr->valstorage[chunk] = xaAlloc(stGetSize(hdr->valtype));
        chunkinfo->nalloc = 1;
    }
    memset(chunkinfo->deleted, 0, sizeof(chunkinfo->deleted));
}

static uint32 _htNewSlot(_Inout_ HashTableHeader *hdr)
{
    uint32 chunk = HT_SLOT_CHUNK(hdr->storused);
    if (!(hdr->flags & (HT_InsertOpt | HT_Compact))) {
        // standard chunk allocation strategy - we need to check if this goes over a quarter-chunk boundary
        if ((hdr->storused & HT_QCHUNK_MASK) == 0 && (hdr->storused & HT_CHUNK_MASK) != 0) {
            // need to resize chunk
            devAssert(hdr->chunks[chunk].nalloc == (hdr->storused & HT_CHUNK_MASK));
            hdr->chunks[chunk].nalloc += HT_QUARTER_CHUNK;
            devAssert(hdr->chunks[chunk].nalloc <= HT_SLOTS_PER_CHUNK);
            xaResize(&hdr->keystorage[chunk], (size_t)hdr->chunks[chunk].nalloc * stGetSize(hdr->keytype));
            if (stGetSize(hdr->valtype) > 0)
                xaResize(&hdr->valstorage[chunk], (size_t)hdr->chunks[chunk].nalloc * stGetSize(hdr->valtype));
        }
    } else if (hdr->flags & HT_Compact) {
        // compact allocation strategy; always resize the chunk for the new item
        // first slot always gets allocated with the chunk though
        if ((hdr->storused & HT_CHUNK_MASK) > 0) {
            devAssert(hdr->chunks[chunk].nalloc == (hdr->storused & HT_CHUNK_MASK));
            hdr->chunks[chunk].nalloc++;
            devAssert(hdr->chunks[chunk].nalloc <= HT_SLOTS_PER_CHUNK);
            xaResize(&hdr->keystorage[chunk], (size_t)hdr->chunks[chunk].nalloc * stGetSize(hdr->keytype));
            if (stGetSize(hdr->valtype) > 0)
                xaResize(&hdr->valstorage[chunk], (size_t)hdr->chunks[chunk].nalloc * stGetSize(hdr->valtype));
        }
    }
    // implied else -- insert-optimized always allocates full chunks, nothing to do here

    uint32 ret = hdr->storused++;
    if ((hdr->storused & HT_CHUNK_MASK) == 0) {
        // crossed the boundary into a new chunk, add one
        _htNewChunk(hdr);
    }
    return ret;
}

_Use_decl_annotations_
uint32 _htNextSlot(HashTableHeader *hdr, uint32 slot)
{
    for (++slot; slot < hdr->storused; ++slot) {
        // skip over completely deallocated chunks
        uint32 chunk = HT_SLOT_CHUNK(slot);
        if (!hdr->keystorage[chunk]) {
            slot = HT_SLOTS_PER_CHUNK * (chunk + 1) - 1;
            continue;
        }

        // skip over deleted slots
        if (hdr->chunks[chunk].deleted[HT_DELETED_IDX(slot)] & HT_DELETED_BIT(slot))
            continue;

        // this is a valid slot!
        return slot;
    }

    return hashIndexEmpty;
}

_Use_decl_annotations_
void _htInit(hashtable *out, stype keytype, STypeOps *keyops, stype valtype, STypeOps *valops, uint32 initsz, flags_t flags)
{
    HashTableHeader *ret;

    relAssert(stGetSize(keytype) > 0);

    if (initsz == 0)
        initsz = HT_QUARTER_CHUNK * 2;
    else
        initsz = clamplow(initsz, 1);

    // If we are not using custom ops and this is a POD type, skip all of the copy/destructor
    // logic and just use straight memory copies for speed.
    if (!keyops && !(stHasFlag(keytype, Object)))
        flags |= HT_RefKeys;
    if (stGetSize(valtype) == 0 || (!valops && !(stHasFlag(valtype, Object))))
        flags |= HT_Ref;

    if ((HT_GET_GROW(flags) & HT_GROW_AT_MASK) != HT_GROW_At50) {
        // At an average table load of 0.5, linear probing is better due to cache.
        // For everything else use quadratic to avoid clustering.
        flags |= HTINT_Quadratic;
    }

    if ((HT_GET_GROW(flags) & HT_GROW_BY_MASK) == HT_GROW_By100 ||
        (HT_GET_GROW(flags) & HT_GROW_BY_MASK) == HT_GROW_By300) {
        // 100% (2x) and 300% (4x) grow settings are special
        // If we set the initial table size to a power of 2, it will always be a
        // power-of-2 sized table even after growing, so we can use fast indexing
        // with masking rather than modulo.
        flags |= HTINT_Pow2;
        initsz = npow2(initsz);
    }

    if (keyops || valops) {
        // need the extended header
        flags |= HTINT_Extended;
        ret = xaAlloc(HTABLE_HDRSIZE + HT_IDXENT_SZ * initsz);
        memset(&ret->keytypeops, 0, sizeof(ret->keytypeops));
        memset(&ret->valtypeops, 0, sizeof(ret->valtypeops));
    } else {
        // revel in the evil
        ret = (HashTableHeader *)((uintptr)xaAlloc((HTABLE_HDRSIZE + HT_IDXENT_SZ * initsz) - HT_SMALLHDR_OFFSET) - HT_SMALLHDR_OFFSET);
    }

    ret->idxsz = initsz;
    ret->idxused = ret->storused = ret->valid = 0;

    ret->keytype = keytype;
    ret->valtype = valtype;
    ret->flags = flags;

    // prep the first storage chunk
    ret->chunks = NULL;
    ret->valstorage = NULL;
    _htNewChunk(ret);
    // we reserve slot 0 to make things like htFind be able to return 0 when not found, which nicely converts to a boolean
    ret->storused = 1;

    if (keyops) {
        ret->keytypeops = *keyops;
    }
    if (valops) {
        ret->valtypeops = *valops;
    }

    // fill index with empty sentinel
    for (uint32 i = 0; i < initsz; i++) {
        ret->index[i] = hashIndexEmpty;
    }

    *out = (hashtable)&ret->index[0];
}

static _meta_inline uint32 clampHash(_In_ HashTableHeader *hdr, uint32 hash)
{
    if (hdr->flags & HTINT_Pow2)
        return hash & (hdr->idxsz - 1);
    else
        return hash % hdr->idxsz;
}

// variant of htFindIndex optimized for reindexing
// just finds the first empty index where this key can fit
static _meta_inline uint32 htFastFindIndex(_In_ HashTableHeader *hdr, _In_ stgeneric key)
{
    uint32 opsflags = (hdr->flags & HT_CaseInsensitive) ? ST_CaseInsensitive : 0;
    uint32 probes = 1;

    uint32 *idx = hdr->index;
    uint32 hash = _stHash(hdr->keytype, HDRKEYOPS(hdr), key, opsflags);

    for (;;) {
        hash = clampHash(hdr, hash);
        uint32 slot = idx[hash];
        if (slot == hashIndexEmpty)
            return hash;

        // keep searching
        if (hdr->flags & HTINT_Quadratic) {
            hash += probes * probes;
            probes++;
        } else {
            // linear probing
            hash++;
        }
    }
}

_Ret_valid_
static hashtable _htClone(_In_ hashtable htbl, uint32 minsz, bool move, bool repack)
{
    HashTableHeader *hdr = _htHdr(htbl);
    HashTableHeader *nhdr;
    hashtable ntbl;

    if (minsz == 0)
        minsz = hdr->idxsz;

    uint32 newsz = clamplow(minsz, hdr->valid + 1);
    if (hdr->flags & HTINT_Pow2)
        newsz = npow2(newsz);

    // make a new table to copy stuff into
    if (hdr->flags & HTINT_Extended) {
        // need the extended header
        nhdr = xaAlloc(HTABLE_HDRSIZE + HT_IDXENT_SZ * newsz);
    } else {
        nhdr = (HashTableHeader *)((uintptr)xaAlloc((HTABLE_HDRSIZE + HT_IDXENT_SZ * newsz) - HT_SMALLHDR_OFFSET) - HT_SMALLHDR_OFFSET);
    }
    ntbl = (hashtable)&nhdr->index[0];

    nhdr->keytype = hdr->keytype;
    nhdr->valtype = hdr->valtype;
    nhdr->idxsz = newsz;
    nhdr->idxused = nhdr->valid = nhdr->storused = 0;

    uint32 origflags = hdr->flags;
    if (move && repack) {
        // neither table should own anything during the copy
        // that way everything is done with straight memcpy
        // this permanently changes the flags on the source table!
        hdr->flags |= HT_Ref | HT_RefKeys;
        nhdr->flags = hdr->flags;
    } else {
        nhdr->flags = hdr->flags;
    }

    if (hdr->flags & HTINT_Extended) {
        nhdr->keytypeops = hdr->keytypeops;
        nhdr->valtypeops = hdr->valtypeops;
    }

    // initialize new index
    for (uint32 i = 0; i < newsz; i++) {
        nhdr->index[i] = hashIndexEmpty;
    }

    if (!move || repack) {
        // if we're cloning or repacking, copy the actual data
        // this also re-indexes as we go
        nhdr->chunks = NULL;
        nhdr->valstorage = NULL;
        _htNewChunk(nhdr);
        nhdr->storused = 1;
        if (stGetSize(hdr->valtype) > 0) {
            for (uint32 slot = _htNextSlot(hdr, 0); slot != 0; slot = _htNextSlot(hdr, slot)) {
                _htInsertInternal(&ntbl, stStored(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)),
                                  stStoredPtr(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), 0);
            }
        } else {
            for (uint32 slot = _htNextSlot(hdr, 0); slot != 0; slot = _htNextSlot(hdr, slot)) {
                _htInsertInternal(&ntbl, stStored(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)), NULL, 0);
            }
        }
        // this *shouldn't* actually change since the new index is preallocated
        devAssert(nhdr == HTABLE_HDR(ntbl));
        nhdr = _htHdr(ntbl);
    } else {
        // we're moving and not repacking, just link to the old storage
        nhdr->chunks = hdr->chunks;
        nhdr->keystorage = hdr->keystorage;
        nhdr->valstorage = hdr->valstorage;
        nhdr->storsz = hdr->storsz;
        nhdr->storused = hdr->storused;

        // and reindex
        uint32 idxent;
        for (uint32 slot = _htNextSlot(hdr, 0); slot != 0; slot = _htNextSlot(hdr, slot)) {
            idxent = htFastFindIndex(nhdr, stStored(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)));
            nhdr->idxused++;
            nhdr->index[idxent] = slot;
        }

        nhdr->valid = hdr->valid;
        devAssert(nhdr->valid == nhdr->idxused);
    }

    if (move) {
        // restore ownership for new table
        nhdr->flags = origflags;
    }

    devAssert(nhdr->valid == hdr->valid);

    return ntbl;
}

_Use_decl_annotations_
void htReindex(hashtable *htbl, uint32 minsz)
{
    HashTableHeader *hdr = _htHdr(*htbl);

    *htbl = _htClone(*htbl, minsz, true, false);

    // free old table
    if (hdr->flags & HTINT_Extended) {
        xaFree(hdr);
    } else {
        void *smbase = (void *)((uintptr_t)hdr + HT_SMALLHDR_OFFSET);
        xaFree(smbase);
    }
}

_Use_decl_annotations_
void htRepack(hashtable *htbl)
{
    HashTableHeader *hdr = _htHdr(*htbl);

    *htbl = _htClone(*htbl, 0, true, true);

    // free all chunks
    for (uint32 chunk = 0; chunk < HT_SLOT_CHUNK(hdr->storused + HT_SLOTS_PER_CHUNK - 1); ++chunk) {
        xaFree(hdr->keystorage[chunk]);
        if (stGetSize(hdr->valtype) > 0)
            xaFree(hdr->valstorage[chunk]);
    }
    xaFree(hdr->keystorage);
    if (stGetSize(hdr->valtype) > 0)
        xaFree(hdr->valstorage);
    xaFree(hdr->chunks);

    // free old table
    if (hdr->flags & HTINT_Extended) {
        xaFree(hdr);
    } else {
        void *smbase = (void *)((uintptr_t)hdr + HT_SMALLHDR_OFFSET);
        xaFree(smbase);
    }
}

static void htGrowIndex(_Inout_ptr_ hashtable *htbl)
{
    HashTableHeader *hdr = _htHdr(*htbl);
    uint32 newsz = hdr->idxsz;

    if (hdr->flags & HTINT_Pow2)
        newsz = npow2(newsz);

    newsz = calcGrowth(hdr, newsz);

    htReindex(htbl, newsz);
}

static bool htFindIndex(_In_ HashTableHeader *hdr, _In_ stgeneric key, _Out_opt_ uint32 *indexOut, _Out_opt_ uint32 *deletedOut)
{
    uint32 opsflags = (hdr->flags & HT_CaseInsensitive) ? ST_CaseInsensitive : 0;
    uint32 probes = 1;

    if (deletedOut)
        *deletedOut = hashIndexDeleted;

    uint32 *idx = hdr->index;
    uint32 hash = _stHash(hdr->keytype, HDRKEYOPS(hdr), key, opsflags);

    for (;;) {
        hash = clampHash(hdr, hash);
        uint32 slot = idx[hash];
        if (slot == hashIndexEmpty) {
            // nothing in this index entry, just return it
            if (indexOut)
                *indexOut = hash;
            return false;
        }

        if (slot != hashIndexDeleted) {
            // not deleted, so check the key
            void *skey = HT_SLOT_KEY_PTR(hdr, slot);
            if (_stCmp(hdr->keytype, HDRKEYOPS(hdr), key,
                       stStored(hdr->keytype, skey), opsflags | ST_Equality) == 0) {
                // found it!
                if (indexOut)
                    *indexOut = hash;
                return true;
            }
        } else {
            // this is a deleted key, keep searching
            // and cache it if it's the first one
            if (deletedOut && *deletedOut == hashIndexEmpty)
                *deletedOut = hash;
        }

        // keep searching
        if (hdr->flags & HTINT_Quadratic) {
            hash += probes * probes;
            probes++;
        } else {
            // linear probing
            hash++;
        }
    }
}

_Success_(return)
static bool htFindSlot(_In_ HashTableHeader *hdr, _In_ stgeneric key, _Out_ uint32 *slotOut)
{
    uint32 idx;
    if (htFindIndex(hdr, key, &idx, NULL)) {
        *slotOut = hdr->index[idx];
        return true;
    }
    return false;
}

_Use_decl_annotations_
void htClone(hashtable *out, hashtable ref)
{
    *out = _htClone(ref, 0, false, false);
}

//static _meta_inline void htSetValueInternal(HashTableHeader *hdr, int32 slot, void *val, bool consume)
static void htSetValueInternal(_Inout_ HashTableHeader *hdr, uint32 slot, _When_(!consume, _In_) _When_(consume, _Pre_notnull_ _Post_invalid_) stgeneric *val, bool consume)
{
    if (consume) {
        // special case: if we're consuming, just steal the element instead of deep copying it,
        // even if we're the owner
        memcpy(HT_SLOT_VAL_PTR(hdr, slot), stGenPtr(hdr->valtype, *val), stGetSize(hdr->valtype));

        // destroy source
        if (hdr->flags & HT_Ref)        // this combination doesn't make much sense, but should respect it
            _stDestroy(hdr->valtype, HDRVALOPS(hdr), val, 0);
        else if (stGetSize(hdr->valtype) == sizeof(void *))
            val->st_ptr = 0;            // if this is a pointer-sized element, clear it out
        return;
    }

    if (!(hdr->flags & HT_Ref))
        _stCopy(hdr->valtype, HDRVALOPS(hdr),
                stStoredPtr(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), *val, 0);
    else
        memcpy(HT_SLOT_VAL_PTR(hdr, slot), stGenPtr(hdr->valtype, *val),
               stGetSize(hdr->valtype));
}

_Use_decl_annotations_
static uint32 _htInsertInternal(hashtable *htbl, stgeneric key, stgeneric *val, flags_t flags)
{
    HashTableHeader *hdr = _htHdr(*htbl);
    uint32 idxent, deleted, slot;
    bool found;

    found = htFindIndex(hdr, key, &idxent, &deleted);

    if (found) {
        // get storage slot for the index
        slot = hdr->index[idxent];

        if (flags & HT_Ignore) {
            // already exists and set to ignore, so do not set value
            if (flags & HTINT_Consume)
                _stDestroy(hdr->valtype, HDRVALOPS(hdr), val, 0);
            return slot;
        }

        if (stGetSize(hdr->valtype) > 0) {
            // replacing existing value, destroy it first if necessary
            if (!(hdr->flags & HT_Ref))
                _stDestroy(hdr->valtype, HDRVALOPS(hdr),
                           stStoredPtr(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), 0);

            htSetValueInternal(hdr, slot, val, flags & HTINT_Consume);
        }
        return slot;
    }

    // didn't find it...

    if (deleted != hashIndexDeleted)
        idxent = deleted;           // reuse deleted index entry instead of a new one
    else
        hdr->idxused++;

    // get a new storage slot
    slot = _htNewSlot(hdr);

    // set key
    if (!(hdr->flags & HT_RefKeys))
        _stCopy(hdr->keytype, HDRKEYOPS(hdr),
                stStoredPtr(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)), key, 0);
    else
        memcpy(HT_SLOT_KEY_PTR(hdr, slot), stGenPtr(hdr->keytype, key), stGetSize(hdr->keytype));

    if (val && stGetSize(hdr->valtype) > 0)         // handle 'none' typed hashed sets
        htSetValueInternal(hdr, slot, val, flags & HTINT_Consume);

    hdr->index[idxent] = slot;
    hdr->valid++;

    // check to see if index needs to be grown
    uint32 growsz;
    switch (HT_GET_GROW(hdr->flags) & HT_GROW_AT_MASK) {
    case HT_GROW_At50:
        growsz = hdr->idxsz >> 1;
        break;
    case HT_GROW_At75:
        growsz = (hdr->idxsz >> 1) + (hdr->idxsz >> 2);
        break;
    case HT_GROW_At90:
        growsz = hdr->idxsz * 9 / 10;
        break;
    default:
        devFatalError("Invalid hashtable grow threshold");
        growsz = hdr->idxsz >> 1;
    }

    if (hdr->idxused >= growsz)
        htGrowIndex(htbl);

    return slot;
}

_Use_decl_annotations_
htelem _htInsertPtr(hashtable *htbl, stgeneric key, stgeneric *val, flags_t flags)
{
    return _htInsertInternal(htbl, key, val, flags);
}

_Use_decl_annotations_
htelem _htInsert(hashtable *htbl, stgeneric key, stgeneric val, flags_t flags)
{
    return _htInsertInternal(htbl, key, &val, flags);
}

static void _htClear(_Inout_ HashTableHeader *hdr, bool reuse)
{
    // destroy all valid elements
    if (!((hdr->flags & HT_Ref) && (hdr->flags & HT_RefKeys))) {
        // destroy data in storage slots
        for (uint32 slot = _htNextSlot(hdr, 0); slot != 0; slot = _htNextSlot(hdr, slot)) {
            if (!(hdr->flags & HT_RefKeys))
                _stDestroy(hdr->keytype, HDRKEYOPS(hdr),
                           stStoredPtr(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)), 0);
            if (!(hdr->flags & HT_Ref))
                _stDestroy(hdr->valtype, HDRVALOPS(hdr),
                           stStoredPtr(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), 0);
        }
    }

    // free all chunks (except for the first one if we're planning to reuse it)
    for (uint32 chunk = reuse ? 1 : 0; chunk < HT_SLOT_CHUNK(hdr->storused + HT_SLOTS_PER_CHUNK - 1); ++chunk) {
        xaFree(hdr->keystorage[chunk]);
        if (stGetSize(hdr->valtype) > 0)
            xaFree(hdr->valstorage[chunk]);
    }
    hdr->storused = 0;

    if (reuse) {
        xaResize(&hdr->chunks, 4 * sizeof(HTChunkInfo));
        xaResize(&hdr->keystorage, 4 * sizeof(void *));
        if (stGetSize(hdr->valtype) > 0)
            xaResize(&hdr->valstorage, 4 * sizeof(void *));
        hdr->storsz = 4;

        if (hdr->keystorage[0]) {
            // recycle first chunk
            memset(hdr->chunks[0].deleted, 0, sizeof(hdr->chunks[0].deleted));
            if (!(hdr->flags & (HT_InsertOpt | HT_Compact))) {
                hdr->chunks[0].nalloc = HT_QUARTER_CHUNK;
            } else if (hdr->flags & HT_InsertOpt) {
                hdr->chunks[0].nalloc = HT_SLOTS_PER_CHUNK;
            } else { // if (hdr->flags & HT_Compact)
                hdr->chunks[0].nalloc = 1;
            }
        } else {
            // it was previously freed, get a new one
            _htNewChunk(hdr);
        }
        hdr->storused = 1;

        // clear index
        for (uint32 i = 0; i < hdr->idxsz; i++) {
            hdr->index[i] = hashIndexEmpty;
        }
    }
    hdr->idxused = 0;
    hdr->valid = 0;
}

_Use_decl_annotations_
void htClear(hashtable *htbl)
{
    if (!(htbl && *htbl))
        return;

    _htClear(_htHdr(*htbl), true);
}

_Use_decl_annotations_
void htDestroy(hashtable *htbl)
{
    if (!(htbl && *htbl))
        return;

    _htClear(_htHdr(*htbl), false);

    HashTableHeader *hdr = _htHdr(*htbl);
    xaFree(hdr->keystorage);
    if (stGetSize(hdr->valtype) > 0)
        xaFree(hdr->valstorage);
    xaFree(hdr->chunks);
    if (hdr->flags & HTINT_Extended) {
        xaFree(hdr);
    } else {
        void *smbase = (void *)((uintptr_t)hdr + HT_SMALLHDR_OFFSET);
        xaFree(smbase);
    }
    *htbl = NULL;
}

_Static_assert(0 == hashIndexEmpty, "0 must equal hashIndexEmpty");
_Use_decl_annotations_
htelem _htFind(hashtable htbl, stgeneric key, stgeneric *val, flags_t flags)
{
    HashTableHeader *hdr = _htHdr(htbl);

    uint32 slot;
    bool found = htFindSlot(hdr, key, &slot);

    if (found) {
        if (val) {
            devAssert(stGetSize(hdr->valtype) > 0);
            if ((flags & HT_Borrow) || (hdr->flags & HT_Ref))
                memcpy(stGenPtr(hdr->valtype, *val), HT_SLOT_VAL_PTR(hdr, slot), stGetSize(hdr->valtype));
            else
                _stCopy(hdr->valtype, HDRVALOPS(hdr),
                        val, stStored(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), 0);
        }
        return slot;
    }

    return hashIndexEmpty;
}

_Use_decl_annotations_
bool _htHasKey(hashtable htbl, stgeneric key)
{
    HashTableHeader *hdr = _htHdr(htbl);
    return htFindIndex(hdr, key, NULL, NULL);
}

_Use_decl_annotations_
bool _htExtract(hashtable *htbl, stgeneric key, stgeneric *val)
{
    HashTableHeader *hdr = _htHdr(*htbl);
    if (!devVerifyMsg(!val || stGetSize(hdr->valtype) > 0, "hashed sets cannot return a value"))
        return false;

    uint32 idxent;
    bool found = htFindIndex(hdr, key, &idxent, NULL);

    if (found) {
        uint32 slot = hdr->index[idxent];
        // either extract the value by stealing any reference, or destroy it
        if (val)
            memcpy(stGenPtr(hdr->valtype, *val), HT_SLOT_VAL_PTR(hdr, slot), stGetSize(hdr->valtype));
        else if (!(hdr->flags & HT_Ref))
            _stDestroy(hdr->valtype, HDRVALOPS(hdr),
                       stStoredPtr(hdr->valtype, HT_SLOT_VAL_PTR(hdr, slot)), 0);
        if (!(hdr->flags & HT_RefKeys))
            _stDestroy(hdr->keytype, HDRKEYOPS(hdr),
                       stStoredPtr(hdr->keytype, HT_SLOT_KEY_PTR(hdr, slot)), 0);

        // mark deleted in index
        hdr->index[idxent] = hashIndexDeleted;
        // mark deleted in storage
        uint32 chunk = HT_SLOT_CHUNK(slot);
        HTChunkInfo *chunkinfo = &hdr->chunks[chunk];
        chunkinfo->deleted[HT_DELETED_IDX(slot)] |= HT_DELETED_BIT(slot);

        hdr->valid--;

        if (hdr->storused > HT_SLOTS_PER_CHUNK && hdr->valid < (hdr->storused >> 2)) {
            // if storage array utilization falls below 25%, compact it
            htRepack(htbl);
        } else if (chunk < HT_SLOT_CHUNK(hdr->storused)){
            // Check if the chunk is completely empty. This check does NOT happen on the last
            // chunk of the array, which may be partially filled.
            devAssert(chunkinfo->nalloc == HT_SLOTS_PER_CHUNK);

            bool left = false;
            for (int i = 0; !left && i < HT_SLOTS_PER_CHUNK >> 3; ++i) {
                if (chunkinfo->deleted[i] != 0xff) {
                    left = true;
                }
            }

            if (!left) {
                // deallocate the empty chunk
                xaFree(hdr->keystorage[chunk]);
                hdr->keystorage[chunk] = NULL;
                if (stGetSize(hdr->valtype) > 0) {
                    xaFree(hdr->valstorage[chunk]);
                    hdr->valstorage[chunk] = NULL;
                }
            }
        }
    }

    return found;
}

_Use_decl_annotations_
bool htiInit(htiter *iter, hashtable htbl)
{
    if (!iter)
        return false;
    if (!(htbl)) {
        iter->hdr = NULL;
        iter->slot = 0;
        return false;
    }

    iter->hdr = _htHdr(htbl);
    iter->slot = 0;
    return htiNext(iter);
}

_Use_decl_annotations_
bool htiNext(htiter *iter)
{
    if (!(iter && iter->hdr))
        return false;

    iter->slot = _htNextSlot(iter->hdr, iter->slot);
    if (iter->slot == hashIndexEmpty) {
        iter->hdr = NULL;
        return false;
    }

    return true;
}

_Use_decl_annotations_
void htiFinish(htiter *iter)
{
    memset(iter, 0, sizeof(htiter));
}

// HT_SLOT_KEY_PTR wrapper with safety checks
_Use_decl_annotations_
void *_hteElemKeyPtr(HashTableHeader *hdr, htelem elem)
{
    if (elem == 0)
        return NULL;

    uint32 chunk = HT_SLOT_CHUNK(elem);
    if (!hdr || elem >= hdr->storused || !hdr->keystorage[chunk])
        return NULL;

    if (hdr->chunks[chunk].deleted[HT_DELETED_IDX(elem)] & HT_DELETED_BIT(elem))
        return NULL;

    // this is a valid slot!
    return HT_SLOT_KEY_PTR(hdr, elem);
}

// HT_SLOT_VAL_PTR wrapper with safety checks
_Use_decl_annotations_
void *_hteElemValPtr(HashTableHeader *hdr, htelem elem)
{
    uint32 chunk = HT_SLOT_CHUNK(elem);
    if (!hdr || elem >= hdr->storused || !hdr->valstorage || !hdr->valstorage[chunk])
        return NULL;

    if (hdr->chunks[chunk].deleted[HT_DELETED_IDX(elem)] & HT_DELETED_BIT(elem))
        return NULL;

    // this is a valid slot!
    return HT_SLOT_VAL_PTR(hdr, elem);
}
