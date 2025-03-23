#ifdef WIN32
#include <windows.h>
#endif

#include <stdbool.h>
#include <stdint.h>
#include "minicrt.h"

#define HT_BITMAP_IDX(slot)         (slot >> 2)
#define HT_BITMAP_USED_BIT(slot)    (1 << ((slot & 3) << 1))
#define HT_BITMAP_DELETED_BIT(slot) (2 << ((slot & 3) << 1))

static uint32_t murmur_seed;

static lazy_init murmur_is_init;
static void initSeed(void* unused)
{
#ifdef WIN32
    murmur_seed = GetTickCount();
#endif
}

uint32_t hashMurmur3(const uint8_t* key, size_t len)
{
    lazyinit(&murmur_is_init, initSeed, NULL);

    uint32_t h = murmur_seed;
    if (len > 3) {
        size_t i = len >> 2;
        do {
            uint32_t k;
            memcpy(&k, key, sizeof(uint32_t));
            key += sizeof(uint32_t);
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
    }
    if (len & 3) {
        size_t i   = len & 3;
        uint32_t k = 0;
        do {
            k <<= 8;
            k |= key[i - 1];
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

static uint32_t hashMurmur3i(const uint8_t* key, size_t len)
{
    lazyinit(&murmur_is_init, initSeed, NULL);

    uint32_t h = murmur_seed;
    if (len > 3) {
        size_t i = len >> 2;
        do {
            uint32_t k;
            memcpy(&k, key, sizeof(uint32_t));
            k |= 0x20202020;
            key += sizeof(uint32_t);
            k *= 0xcc9e2d51;
            k = (k << 15) | (k >> 17);
            k *= 0x1b873593;
            h ^= k;
            h = (h << 13) | (h >> 19);
            h = h * 5 + 0xe6546b64;
        } while (--i);
    }
    if (len & 3) {
        size_t i   = len & 3;
        uint32_t k = 0;
        do {
            k <<= 8;
            k |= key[i - 1] | 0x20;
        } while (--i);
        k *= 0xcc9e2d51;
        k = (k << 15) | (k >> 17);
        k *= 0x1b873593;
        h ^= k;
    }
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}

void hashtbl_init(hashtbl* tbl, int initsz, int flags)
{
    tbl->nslots = initsz;
    tbl->used   = 0;
    tbl->ents   = smalloc(initsz * sizeof(hashtbl_ent) + ((initsz + 3) >> 2));
    tbl->bitmap = (uint8_t*)tbl->ents + initsz * sizeof(hashtbl_ent);
    memset(tbl->bitmap, 0, (initsz + 3) >> 2);
    tbl->flags  = flags;
}

static bool ht_find_internal(hashtbl* tbl, uintptr_t key, uint32_t* slot, uint32_t* deleted)
{
    uint32_t hash;

    if (deleted)
        *deleted = HT_NOT_FOUND;

    if (tbl->flags & HT_STRING_KEYS) {
        const char* str = (const char*)key;
        if (tbl->flags & HT_CASE_INSENSITIVE) {
            hash = hashMurmur3i(str, strlen(str));
        } else {
            hash = hashMurmur3(str, strlen(str));
        }
    } else {
        hash = hashMurmur3((uint8_t*)&key, sizeof(uintptr_t));
    }

    for (;;) {
        hash = hash % tbl->nslots;
        if (!(tbl->bitmap[HT_BITMAP_IDX(hash)] & HT_BITMAP_USED_BIT(hash))) {
            // empty index
            if (slot)
                *slot = hash;
            return false;
        }

        if (!(tbl->bitmap[HT_BITMAP_IDX(hash)] & HT_BITMAP_DELETED_BIT(hash))) {
            // not deleted, check the key
            bool match = false;
            if (tbl->flags & HT_STRING_KEYS) {
                const char* str = (const char*)key;
                if (tbl->flags & HT_CASE_INSENSITIVE) {
                    match = !stricmp(str, tbl->ents[hash].key_str);
                } else {
                    match = !strcmp(str, tbl->ents[hash].key_str);
                }
            } else {
                match = (key == tbl->ents[hash].key_int);
            }
            if (match) {
                if (slot)
                    *slot = hash;
                return true;
            }
        } else {
            if (deleted && *deleted == HT_NOT_FOUND)
                *deleted = hash;
        }

        // keep searching
        hash++;
    }
}

static uint32_t ht_insert_internal(hashtbl* tbl, uintptr_t key, void* data, bool overwrite)
{
    uint32_t slot, deleted;
    bool found = ht_find_internal(tbl, key, &slot, &deleted);

    if (found) {
        if (!overwrite)
            return HT_NOT_FOUND;
        tbl->ents[slot].key_int = key;   // works for string keys as well, just a pointer copy
        tbl->ents[slot].data    = data;
        return slot;
    } else if (deleted != HT_NOT_FOUND) {
        slot = deleted;
        tbl->used++;
    } else {
        tbl->used++;
    }

    tbl->ents[slot].key_int = key;
    tbl->ents[slot].data    = data;
    tbl->bitmap[HT_BITMAP_IDX(slot)] &= ~HT_BITMAP_DELETED_BIT(slot);
    tbl->bitmap[HT_BITMAP_IDX(slot)] |= HT_BITMAP_USED_BIT(slot);
    return slot;
}

static void ht_grow(hashtbl* tbl)
{
    hashtbl_ent* oldents = tbl->ents;
    uint8_t* oldbitmap   = tbl->bitmap;
    uint32_t oldused     = tbl->used;
    uint32_t oldslots    = tbl->nslots;

    tbl->nslots = oldslots << 1;
    tbl->used   = 0;
    tbl->ents   = smalloc(tbl->nslots * sizeof(hashtbl_ent) + ((tbl->nslots + 3) >> 2));
    tbl->bitmap = (uint8_t*)tbl->ents + tbl->nslots * sizeof(hashtbl_ent);
    memset(tbl->bitmap, 0, (tbl->nslots + 3) >> 2);

    for (uint32_t i = 0; i < oldslots; ++i) {
        if (oldbitmap[HT_BITMAP_IDX(i)] & HT_BITMAP_USED_BIT(i) &&
            !(oldbitmap[HT_BITMAP_IDX(i)] & HT_BITMAP_DELETED_BIT(i))) {
            ht_insert_internal(tbl, oldents[i].key_int, oldents[i].data, false);
        }
    }

    // assert(tbl->used == oldused);

    sfree(oldents);
}

uint32_t _hashtbl_add(hashtbl* tbl, uintptr_t key, void* data)
{
    if (tbl->used * 3 > tbl->nslots * 2)
        ht_grow(tbl);
    return ht_insert_internal(tbl, key, data, false);
}

uint32_t _hashtbl_set(hashtbl* tbl, uintptr_t key, void* data)
{
    if (tbl->used * 3 > tbl->nslots * 2)
        ht_grow(tbl);
    return ht_insert_internal(tbl, key, data, true);
}

uint32_t _hashtbl_find(hashtbl* tbl, uintptr_t key)
{
    uint32_t slot;
    if (ht_find_internal(tbl, key, &slot, NULL))
        return slot;
    return HT_NOT_FOUND;
}

void* _hashtbl_get(hashtbl* tbl, uintptr_t key)
{
    uint32_t slot;
    if (ht_find_internal(tbl, key, &slot, NULL))
        return tbl->ents[slot].data;
    return NULL;
}

bool _hashtbl_getint(hashtbl* tbl, uintptr_t key, uintptr_t *pval)
{
    uint32_t slot;
    if (ht_find_internal(tbl, key, &slot, NULL)) {
        *pval = (uintptr_t)tbl->ents[slot].data;
        return true;
    }
    return false;
}

void* _hashtbl_del(hashtbl* tbl, uintptr_t key)
{
    uint32_t slot;
    void* ret = NULL;
    if (ht_find_internal(tbl, key, &slot, NULL)) {
        ret = tbl->ents[slot].data;
        tbl->bitmap[HT_BITMAP_IDX(slot)] |= HT_BITMAP_DELETED_BIT(slot);
    }
    return ret;
}

void hashtbl_destroy(hashtbl* tbl)
{
    sfree(tbl->ents);
    tbl->bitmap = NULL;
    tbl->ents   = NULL;
    tbl->nslots = 0;
    tbl->used   = 0;
    tbl->flags  = 0;
}
