#pragma once

#include <stdarg.h>

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

void* hmalloc(unsigned int sz);
void* hcalloc(unsigned int num, unsigned int sz);
void* hrealloc(void* ptr, unsigned int sz);
char* hstrdup(const char* src);
void hfree(void* ptr);
#ifdef USE_HMALLOC
#define malloc  hmalloc
#define calloc  hcalloc
#define realloc hrealloc
#define free    hfree
#define strdup  hstrdup
#define _strdup hstrdup
#endif
