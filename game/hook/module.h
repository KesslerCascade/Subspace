#pragma once

#include <cx/container.h>
#include <cx/string.h>
#include "hook/moduleinfo.h"
#include "loader/loader.h"

typedef sa_uintptr AddrList;

AddrList* addrListFindByPtr(hashtable ht, addr_t addr);
AddrList* addrListFindByStr(hashtable ht, strref str);
void addrListAddByPtr(hashtable* ht, addr_t addr, addr_t val);
void addrListAddByStr(hashtable* ht, strref str, addr_t val);

ModuleInfo* moduleInfo(addr_t base);

bool analyzeModule(addr_t base, ModuleInfo* mi);

// free analysis info for all modules and block future use
void cleanupAnalysis();