#pragma once

#include "hashtable.h"
#include "cx/utils.h"

typedef struct HTChunkInfo {
    uint8 deleted[HT_SLOTS_PER_CHUNK >> 3];     // deleted slot bitmap
    uint8 nalloc;                               // how many slots have been allocated for this chunk so far
} HTChunkInfo;

_Static_assert((HT_SLOTS_PER_CHUNK >> 8) < sizeof(((HTChunkInfo*)0)->nalloc), "HT_SLOTS_PER_CHUNK too high to fit into nalloc");

#define HT_IDXENT_SZ (sizeof(uint32))
#define HT_SLOT_CHUNK(slot) ((slot) >> HT_CHUNK_SHIFT)
#define HT_SLOT_KEY_CHUNK_PTR(hdr, slot) ((uintptr)hdr->keystorage[HT_SLOT_CHUNK(slot)])
#define HT_SLOT_VAL_CHUNK_PTR(hdr, slot) ((uintptr)hdr->valstorage[HT_SLOT_CHUNK(slot)])
#define HT_SLOT_OFF(slot, elemsz) ((uintptr)((slot) & HT_CHUNK_MASK) * elemsz)
#define HT_SLOT_KEY_PTR(hdr, slot) ((void*)(HT_SLOT_KEY_CHUNK_PTR(hdr, slot) + (size_t)((slot) & HT_CHUNK_MASK) * stGetSize(hdr->keytype)))
#define HT_SLOT_VAL_PTR(hdr, slot) ((void*)(HT_SLOT_VAL_CHUNK_PTR(hdr, slot) + (size_t)((slot) & HT_CHUNK_MASK) * stGetSize(hdr->valtype)))

#define HT_DELETED_IDX(slot) ((slot & HT_CHUNK_MASK) >> 3)
#define HT_DELETED_BIT(slot) (1 << (slot & 7))

#define HT_SMALLHDR_OFFSET (offsetof(HashTableHeader, idxsz))
#define HDRKEYOPS(hdr) ((hdr->flags & HTINT_Extended) ? &hdr->keytypeops : NULL)
#define HDRVALOPS(hdr) ((hdr->flags & HTINT_Extended) ? &hdr->valtypeops : NULL)

#define hashIndexDeleted (0xffffffffUL)
#define hashIndexEmpty   (0UL)

uint32 _htNextSlot(_Inout_ HashTableHeader *hdr, uint32 slot);
