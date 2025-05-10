// very simple allocator that doesn't need the C runtime
// it's not particularly fast
#ifdef WIN32
#include <windows.h>
#endif
#include <stdbool.h>
#include "minicrt.h"

// default chunk size to request
#define ALLOC_CHUNK_SZ (4096 * 1024)

#ifndef align_up
#define align_up(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#endif

typedef struct sm_blk_t sm_blk_t;
typedef struct sm_blk_t {
    sm_blk_t* next;
    size_t size;
    char userdata[];
} sm_blk_t;

#define SM_CHUNK_MAGIC (0xde981f44)
typedef struct sm_chunk_t sm_chunk_t;
typedef struct sm_chunk_t {
    uintptr_t magic;   // uintptr_t instead of int to keep header pointer-aligned
    sm_chunk_t* next;
    size_t size;
    sm_blk_t* freelist;
    char blocks[];
} sm_chunk_t;

sm_heap_t default_heap;

#define BLOCK_OFFSET ((size_t)&(((sm_blk_t*)0)->userdata))
#define CHUNK_OFFSET ((size_t)&(((sm_chunk_t*)0)->blocks))
#define MIN_ALLOC_SZ (16)

// only lower 32 bits of state are used, but on 64-bit platforms we want uintptr_t to keep
// structures aligned
#define LCG_MAX (0x7ffffffe)
static int lcg_random(_Inout_ uintptr_t* _state)
{
    unsigned int* state = (unsigned int*)_state;
    return ((*state = *state * 1103515245 + 12345) % ((unsigned int)LCG_MAX + 1));
}

/* OS DEPENDENT SECTION - Windows */
#ifdef WIN32

#define WIN32_MIN_CHUNK (1024 * 1024)
static sm_chunk_t* sm_win32_chunk_alloc(size_t* size_inout)
{
    // round up to nearst 1MB
    *size_inout = align_up(*size_inout, WIN32_MIN_CHUNK);

    return (sm_chunk_t*)VirtualAlloc(NULL, *size_inout, MEM_COMMIT, PAGE_READWRITE);
}

static void sm_win32_chunk_free(sm_chunk_t* chunk)
{
    // assert(chunk->magic == SM_CHUNK_MAGIC);
    VirtualFree(chunk, 0, MEM_RELEASE);
}

static void _sm_os_init(void* unused)
{
    sm_init_heap(&default_heap, sm_win32_chunk_alloc, sm_win32_chunk_free);
}

sm_chunk_alloc_t chunkalloc_default = sm_win32_chunk_alloc;
sm_chunk_free_t chunkfree_default = sm_win32_chunk_free;
#endif
/* OS DEPENDENT SECTION ENDS */

static lazy_init sm_is_init;

static void sm_init(void)
{
    lazyinit(&sm_is_init, _sm_os_init, NULL);
}

static void freelist_insert(sm_chunk_t* chunk, sm_blk_t* blk)
{
    sm_blk_t** prevnext = &chunk->freelist;
    sm_blk_t* cur       = chunk->freelist;

    // keep the freelist sorted by address
    while (cur && (uintptr_t)blk > (uintptr_t)cur) {
        prevnext = &cur->next;
        cur      = *prevnext;
    }

    // insert into list (or append if at the tail, cur will be NULL)
    blk->next = cur;
    *prevnext = blk;
}

static void get_new_chunk(sm_heap_t* heap, size_t needed)
{
    // enforce minumum chunk size
    size_t size = MAX(needed, ALLOC_CHUNK_SZ);

    sm_chunk_t* chunk = heap->chunkalloc(&size);
    if (chunk) {
        chunk->magic = SM_CHUNK_MAGIC;
        chunk->size  = size - CHUNK_OFFSET;

        // populate the chunk with a single freelist entry that covers the entire usable space
        sm_blk_t* blk   = (sm_blk_t*)&chunk->blocks;
        blk->next       = NULL;
        blk->size       = chunk->size - BLOCK_OFFSET;
        chunk->freelist = blk;

        // add to the head of the list so that alloc checks there first
        chunk->next     = heap->chunklist;
        heap->chunklist = chunk;
    }
}

void sm_init_heap(sm_heap_t* heap, sm_chunk_alloc_t chunkalloc, sm_chunk_free_t chunkfree)
{
    heap->chunkalloc = chunkalloc;
    heap->chunkfree  = chunkfree;
    heap->chunklist  = NULL;
    lock_init(&heap->lock);
}

void* smalloc_heap(sm_heap_t* heap, unsigned int sz)
{
    void* ret = NULL;

    sm_init();

    sz = align_up(MAX(sz, MIN_ALLOC_SZ), sizeof(void*));   // align everything to pointer size

    // Try twice, once just checking freelist, then another after adding OS memory
    lock_acq(&heap->lock);
    for (int _retry = 0; !ret && _retry < 2; _retry++) {
        // just walk the freelist to try to find a block that's big enough.
        // inefficient, but ok for very basic usage
        sm_chunk_t* chunk = heap->chunklist;
        while (!ret && chunk) {
            sm_blk_t** prevnext = &chunk->freelist;
            sm_blk_t* cur       = chunk->freelist;

            while (cur) {
                if (cur->size >= sz) {
                    // found a spot for it
                    // get pointer to return to caller
                    ret = &cur->userdata;

                    // can we split the block?
                    if (cur->size - sz >= MIN_ALLOC_SZ + BLOCK_OFFSET) {
                        // replace it with split block
                        sm_blk_t* newblk = (sm_blk_t*)((uintptr_t)ret + sz);
                        newblk->size     = cur->size - sz - BLOCK_OFFSET;
                        newblk->next     = cur->next;
                        *prevnext        = newblk;
                        cur->size        = sz;
                    } else {
                        // remove it from the list
                        *prevnext = cur->next;
                    }
                    cur->next = NULL;

                    break;
                }

                prevnext = &cur->next;
                cur      = *prevnext;
            }

            chunk = chunk->next;
        }

        // first time around grab some more RAM
        if (!ret && _retry == 0)
            get_new_chunk(heap, sz + BLOCK_OFFSET + CHUNK_OFFSET);
    }
    lock_rel(&heap->lock);
    return ret;
}

void* scalloc_heap(sm_heap_t* heap, unsigned int num, unsigned int sz)
{
    void* ret = smalloc_heap(heap, num * sz);
    if (ret)
        memset(ret, 0, num * (size_t)sz);
    return ret;
}

void* srealloc_heap(sm_heap_t* heap, void* ptr, unsigned int sz)
{
    if (!ptr)
        return smalloc_heap(heap, sz);

    sm_blk_t* blk = (sm_blk_t*)((uintptr_t)ptr - BLOCK_OFFSET);

    // When shrinking, do a copy if we'd free up at least MIN_ALLOC_SZ bytes.
    // Always doing a copy isn't great but is actually the best case for a simple
    // freelist like this because it prevents excessive fragmentation.
    if (sz <= blk->size && blk->size - sz < MIN_ALLOC_SZ)
        return ptr;

    // TODO: be less lazy and actually check the freelist for an adjacent block for expansion

    void* newptr = smalloc_heap(heap, sz);
    if (newptr) {
        memcpy(newptr, ptr, MIN(sz, blk->size));
        sfree_heap(heap, ptr);
    }
    return newptr;
}

char* sstrdup_heap(sm_heap_t* heap, const char* src)
{
    size_t sz  = strlen(src);
    void* dest = smalloc_heap(heap, sz + 1);
    if (dest) {
        memcpy(dest, src, sz + 1);
    }
    return dest;
}

void sfree_heap(sm_heap_t* heap, void* ptr)
{
    if (!ptr)
        return;

    sm_init();

    sm_blk_t* blk = (sm_blk_t*)((uintptr_t)ptr - BLOCK_OFFSET);

    lock_acq(&heap->lock);
    // find the chunk that this block belongs to
    sm_chunk_t** chunkprevnext = &heap->chunklist;
    sm_chunk_t* chunk          = heap->chunklist;
    while (chunk) {
        if ((uintptr_t)blk > (uintptr_t)chunk &&
            (uintptr_t)blk < (uintptr_t)(&chunk->blocks) + chunk->size) {
            sm_blk_t* prev      = NULL;
            sm_blk_t** prevnext = &chunk->freelist;
            sm_blk_t* cur       = chunk->freelist;

            // find where to put it
            while (cur && (uintptr_t)blk > (uintptr_t)cur) {
                prev     = cur;
                prevnext = &cur->next;
                cur      = *prevnext;
            }

            // can we just extend previous block?
            if (prev && (uintptr_t)blk == (uintptr_t)(&prev->userdata) + prev->size) {
                prev->size += blk->size + BLOCK_OFFSET;
                blk = prev;   // for coalesce check below
            } else {
                // otherwise insert it back in
                blk->next = cur;
                *prevnext = blk;
            }

            // try to coalesce with next block
            if ((uintptr_t)(&blk->userdata) + blk->size == (uintptr_t)blk->next) {
                blk->size += blk->next->size + BLOCK_OFFSET;
                blk->next = blk->next->next;
            }

            // is the entire chunk now empty?
            // note that we're guaranteed at least one freelist entry -- the one we just added
            if (chunk->freelist->next == NULL && chunk->freelist->size + BLOCK_OFFSET == chunk->size) {
                *chunkprevnext = chunk->next;
                heap->chunkfree(chunk);
            }

            break;
        }
        chunkprevnext = &chunk->next;
        chunk         = *chunkprevnext;
    }

    // if chunk is NULL it means we were passed a pointer that doesn't belong to this heap!
    // assert(chunk);

    lock_rel(&heap->lock);
}
