#pragma once

#include <cx/cx.h>

typedef struct HostID {
    uint64 id[4];
    int32 source;       // platform-specific
} HostID;

CX_C void hostId(_Out_ HostID *id);
