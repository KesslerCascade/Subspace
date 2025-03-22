#include "loader/loader.h"
#include "minicrt.h"

typedef void (*modinfo_initfunc_t)(addr_t base, hashtbl* tbl);

hashtbl* moduleExportHash(addr_t base, modinfo_initfunc_t initfunc);
hashtbl* moduleRelcallHash(addr_t base, modinfo_initfunc_t initfunc);
hashtbl* moduleStringHash(addr_t base, modinfo_initfunc_t initfunc);
