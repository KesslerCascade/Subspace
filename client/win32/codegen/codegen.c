#include <windows.h>

#include "codegen_private.h"

#include "minicrt.h"

sm_heap_t codegen_heap;
static lazy_init codegen_is_init;

// Variants of the smalloc chunk Win32 allocator that mark the pages executable

#ifndef align_up
#define align_up(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
#endif
#define WIN32_MIN_CHUNK (1024 * 1024)
static sm_chunk_t* codegen_chunk_alloc(size_t* size_inout)
{
    // round up to nearst 1MB
    *size_inout = align_up(*size_inout, WIN32_MIN_CHUNK);

    return (sm_chunk_t*)VirtualAlloc(NULL, *size_inout, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
}

static void codegen_chunk_free(sm_chunk_t* chunk)
{
    VirtualFree(chunk, 0, MEM_RELEASE);
}

static void codegen_init(void *unused)
{
    sm_init_heap(&codegen_heap, codegen_chunk_alloc, codegen_chunk_free);
}

bool cgInit(void)
{
    lazyinit(&codegen_is_init, codegen_init, NULL);
    return true;
}
