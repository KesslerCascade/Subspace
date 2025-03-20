#pragma once
#include "imgutil.h"

#include <windows.h>

addr_t loadExe(const char* filename);
int importResources(addr_t base);
int importTLSInitializers(addr_t base);

typedef int (*SectionCallback)(addr_t imagebase, IMAGE_SECTION_HEADER* sh, void* userdata);
int enumSections(addr_t imagebase, SectionCallback scb, void* userdata);
IMAGE_SECTION_HEADER* getSection(addr_t imagebase, const char* name);
addr_t getSymbol(addr_t imagebase, const char* name);

#define OVERRIDE_ALL      0x80000000
#define OVERRIDE_ALL_MASK 0x7fffffff
#define overrideAll(x)    ((x & OVERRIDE_ALL) != 0)
int overrideModule(const char* name);
DWORD overrideProcAddress(int module, const char* name);
DWORD overrideProcAddressOrdinal(int ovrmodule, DWORD ordinal);

#define doshdr(b)            ((IMAGE_DOS_HEADER*)b)
#define nthdr(b)             ((IMAGE_NT_HEADERS*)rva(b, doshdr(b)->e_lfanew))
#define datadir(b, t)        (nthdr(b)->OptionalHeader.DataDirectory[t])
#define datadirptr(b, dd, t) ((t*)rva(b, (dd)->VirtualAddress))
#define datadirend(b, dd, t) ((t*)rva(b, (dd)->VirtualAddress + (dd)->Size))
