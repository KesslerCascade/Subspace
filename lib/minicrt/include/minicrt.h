#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// Crappy fill-ins for useful standard library functions that we can't depend on
// the compiler reliably providing intrinsics for.

#ifndef MINICRT_NO_STRING

char* _minicrt_strcat(char* d, const char* src);
#define strcat _minicrt_strcat
char* _minicrt_strcpy(char* d, const char* src);
#define strcpy _minicrt_strcpy
int _minicrt_strcmp(const char* str1, const char* str2);
#define strcmp _minicrt_strcmp
int _minicrt_strncmp(const char* str1, const char* str2, unsigned int count);
#define strncmp _minicrt_strncmp
int _minicrt_stricmp(const char* str1, const char* str2);
#define stricmp _minicrt_stricmp
char* _minicrt_strrchr(const char* p, int ch);
#define strrchr _minicrt_strrchr

char* _minicrt_strtok_r(char* s, const char* delim, char** last);
#define strtok_r _minicrt_strtok_r
char* _minicrt_strtok(char* s, const char* delim);
#define strtok _minicrt_strtok

int _minicrt_atoi(const char* str);
#define atoi _minicrt_
long _minicrt_strtol(const char* nptr, char** endptr, int base);
#define strtol _minicrt_strtol

void* _minicrt_memcpy(void* dst, const void* src, unsigned int count);
#define memcpy _minicrt_memcpy
void* _minicrt_memmove(void* dst, const void* src, unsigned int count);
#define memmove _minicrt_memmove
unsigned int _minicrt_strlen(const char* str);
#define strlen _minicrt_strlen
unsigned int _minicrt_wcslen(const wchar_t* str);
#define wcslen _minicrt_wcslen
int _minicrt_memcmp(const void* s1, const void* s2, unsigned int count);
#define memcmp _minicrt_memcmp
void* _minicrt_memset(void* s, int c, unsigned int count);
#define memset _minicrt_memset

int _minicrt_strnicmp(const char* str1, const char* str2, unsigned int count);
#define strnicmp _minicrt_strnicmp
int _minicrt_memicmp(const void* s1, const void* s2, unsigned int count);
#define memicmp _minicrt_memicmp

char* _minicrt_strlwr(char* str);
#define strlwr _minicrt_strlwr
char* _minicrt_strupr(char* str);
#define strupr _minicrt_strupr
#endif

#if !defined(MINICRT_NO_PRINTF) && !defined(MINICRT_NO_STRING)
// The 'n' variants are not quite standards compliant. They always null-terminate
// the resulting string, making them safer and generally more useful.
int _minicrt_sprintf(char* buff, const char* fmt, ...);
int _minicrt_snprintf(char* buff, unsigned int len, const char* fmt, ...);
int _minicrt_vsnprintf(char* buff, unsigned int len, const char* fmt, va_list arp);
#define sprintf  _minicrt_sprintf
#define snprintf _minicrt_snprintf
#ifdef vsnprintf
#undef vsnprintf
#endif
#define vsnprintf _minicrt_vsnprintf

int _minicrt_wsprintf(wchar_t* buff, const wchar_t* fmt, ...);
int _minicrt_wsnprintf(wchar_t* buff, unsigned int len, const wchar_t* fmt, ...);
int _minicrt_wvsnprintf(wchar_t* buff, unsigned int len, const wchar_t* fmt, va_list arp);
#ifdef wsprintf
#undef wsprintf
#endif
#define wsprintf   _minicrt_wsprintf
#define wsnprintf  _minicrt_wsnprintf
#define wvsnprintf _minicrt_wvsnprintf
#endif

long double pow10l(int p);

#ifdef _WIN32
// Really we should pull in windows.h for CRITICAL_SECTION, but not everybody wants windows.h all
// the time. So fudge it and hardcode in a size that's big enough. Since executables compiled for XP
// continue to work even in Win10, we can assume that 24 bytes is enough.
typedef unsigned char lock_t[24];
#endif

typedef struct sm_blk_t sm_blk_t;
typedef struct sm_chunk_t sm_chunk_t;
typedef sm_chunk_t* (*sm_chunk_alloc_t)(size_t* size_inout);
typedef void (*sm_chunk_free_t)(sm_chunk_t* chunk);
typedef struct sm_heap_t {
    sm_chunk_t* chunklist;
    sm_chunk_alloc_t chunkalloc;
    sm_chunk_free_t chunkfree;
    uintptr_t rngstate;
    lock_t lock;
} sm_heap_t;
extern sm_heap_t default_heap;

void sm_init_heap(sm_heap_t* heap, sm_chunk_alloc_t chunkalloc, sm_chunk_free_t chunkfree);
void* smalloc_heap(sm_heap_t* heap, unsigned int sz);
#define smalloc(sz) smalloc_heap(&default_heap, sz)
void* scalloc_heap(sm_heap_t* heap, unsigned int num, unsigned int sz);
#define scalloc(num, sz) scalloc_heap(&default_heap, num, sz)
void* srealloc_heap(sm_heap_t* heap, void* ptr, unsigned int sz);
#define srealloc(ptr, sz) srealloc_heap(&default_heap, ptr, sz)
char* sstrdup_heap(sm_heap_t* heap, const char* src);
#define sstrdup(src) sstrdup_heap(&default_heap, src)
void sfree_heap(sm_heap_t* heap, void* ptr);
#define sfree(ptr) sfree_heap(&default_heap, ptr)
#ifdef USE_SALLOC
#define malloc  smalloc
#define calloc  scalloc
#define realloc srealloc
#define free    sfree
#define strdup  sstrdup
#define _strdup sstrdup
#endif

void lock_init(lock_t* lck);
void lock_acq(lock_t* lck);
void lock_rel(lock_t* lck);

typedef struct lazy_init {
    unsigned char init;
    unsigned char initProgress;
} lazy_init;

typedef void (*lazy_init_callback_t)(void* user);

void lazyinit_internal(unsigned char* init, unsigned char* initProgress,
                       lazy_init_callback_t initfunc, void* user);

static inline void lazyinit(lazy_init* state, lazy_init_callback_t initfunc, void* user)
{
    if (!state->init)
        lazyinit_internal(&state->init, &state->initProgress, initfunc, user);
}
