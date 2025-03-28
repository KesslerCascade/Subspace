#include "dbgtypes.h"
#include <cx/cx.h>

#ifdef _COMPILER_MSVC
#pragma pack(1)
#define _PACKED
#else
#define _PACKED __attribute__((packed))
#endif

typedef union _nv_stype {
    uint32 spec;
    struct {
        uint8 id;
        uint8 flags;
        uint16 size;
    } _PACKED;
} _nv_stype;

typedef struct _nv_stype_flags {
    void* unused;
} _nv_stype_flags;

typedef struct _nv_string {
    void *unused;
} _nv_string;

typedef struct _nv_string_flags {
    void* unused;
} _nv_string_flags;

typedef struct _nv_sarray {
    void* unused;
} _nv_sarray;

typedef struct _nv_sarray_flags {
    void* unused;
} _nv_sarray_flags;

typedef struct _nv_hashtable {
    void* unused;
} _nv_hashtable;

typedef struct _nv_hashtable_data {
    void* unused;
} _nv_hashtable_data;

typedef struct _nv_hashtable_flags {
    void* unused;
} _nv_hashtable_flags;

// This union ensure that all of the types are "needed" and can't be optmized out during the link
// step. It's a union of pointers in order to take minimal space.

_debug_types _unused_debug_types = { 0 };
