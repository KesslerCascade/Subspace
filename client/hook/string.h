#pragma once

#include "hook/hook.h"
#include "loader/loader.h"

addr_t findString(addr_t base, const char* str);
AddrList* findAllStrings(addr_t base, const char* str);
