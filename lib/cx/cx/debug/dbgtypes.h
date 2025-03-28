#pragma once

#include <cx/stype/stype.h>

// Types that are used for debugging visualization with natvis.
// These are not actually used but are defined in this file so that they are available in the debug
// symbols.

// Many of these are used as pseudo-synthetic types to work around the fact that the gdb natvis
// integration does not support the Synthetic element.

typedef union _nv_stype _nv_stype;
#define NVTYPE(name) typedef struct name name;
NVTYPE(_nv_stype_flags);
NVTYPE(_nv_string);
NVTYPE(_nv_string_flags);
NVTYPE(_nv_sarray);
NVTYPE(_nv_sarray_flags);
NVTYPE(_nv_hashtable);
NVTYPE(_nv_hashtable_data);
NVTYPE(_nv_hashtable_flags);

#undef NVTYPE
#define NVTYPE(name) name* name##_var
typedef union _debug_types {
    void* _unused;
    _nv_stype* _nv_stype_var;
    NVTYPE(_nv_stype_flags);
    NVTYPE(_nv_string);
    NVTYPE(_nv_string_flags);
    NVTYPE(_nv_sarray);
    NVTYPE(_nv_sarray_flags);
    NVTYPE(_nv_hashtable);
    NVTYPE(_nv_hashtable_data);
    NVTYPE(_nv_hashtable_flags);
} _debug_types;

extern _debug_types _unused_debug_types;
