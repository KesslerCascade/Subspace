#pragma once

#include <stdarg.h>

// Crappy fill-ins for useful standard library functions that we can't depend on
// the compiler reliably providing intrinsics for.

#ifndef MINICRT_NO_STRING

char* strcat(char* d, const char* src);
#pragma intrinsic(strcat)
char* strcpy(char* d, const char* src);
#pragma intrinsic(strcpy)
int strcmp(const char* str1, const char* str2);
#pragma intrinsic(strcmp)
int strncmp(const char* str1, const char* str2, unsigned int count);
int stricmp(const char* str1, const char* str2);
char* strrchr(const char* p, int ch);

char* strtok_r(char* s, const char* delim, char** last);
char* strtok(char* s, const char* delim);

int atoi(const char* str);
long strtol(const char* nptr, char** endptr, int base);

void* memcpy(void* dst, const void* src, unsigned int count);
#pragma intrinsic(memcpy)
void* memmove(void* dst, const void* src, unsigned int count);
unsigned int strlen(const char* str);
#pragma intrinsic(strlen)
unsigned int wcslen(const wchar_t* str);
#pragma intrinsic(wcslen)
int memcmp(const void* s1, const void* s2, unsigned int count);
#pragma intrinsic(memcmp)
void* memset(void* s, int c, unsigned int count);
#pragma intrinsic(memset)

int _stricmp(const char* str1, const char* str2);
#define stricmp _stricmp
int _strnicmp(const char* str1, const char* str2, unsigned int count);
#define strnicmp _strnicmp
int _memicmp(const void* s1, const void* s2, unsigned int count);
#define memicmp _memicmp

char* _strlwr(char* str);
#define strlwr _strlwr
char* _strupr(char* str);
#define strupr _strupr
#endif

#if !defined(MINICRT_NO_PRINTF) && !defined(MINICRT_NO_STRING)
// The 'n' variants are not quite standards compliant. They always null-terminate
// the resulting string, making them safer and generally more useful.
int sprintf(char* buff, const char* fmt, ...);
int snprintf(char* buff, unsigned int len, const char* fmt, ...);
int vsnprintf(char* buff, unsigned int len, const char* fmt, va_list arp);

int _wsprintf(wchar_t* buff, const wchar_t* fmt, ...);
int _wsnprintf(wchar_t* buff, unsigned int len, const wchar_t* fmt, ...);
int _wvsnprintf(wchar_t* buff, unsigned int len, const wchar_t* fmt, va_list arp);
#ifdef wsprintf
#undef wsprintf
#endif
#define wsprintf   _wsprintf
#define wsnprintf  _wsnprintf
#define wvsnprintf _wvsnprintf
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
