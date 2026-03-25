#pragma once

#include "hook/hook.h"
#include "hook/module.h"
#include "loader/loader.h"

addr_t findString(addr_t base, strref str);
AddrList* findAllStrings(addr_t base, strref str);
