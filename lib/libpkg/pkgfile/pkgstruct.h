#pragma once

#include <cx/cx.h>

typedef struct PkgHeaderDisk {
    uint8 magic[4];
    uint16 hdrsz;    // header size
    uint16 entsz;    // size of directory entry
    uint32 nent;     // number of directory entries
    uint32 namesz;   // size of name list
} PkgHeaderDisk;

typedef struct PkgDirEntDisk {
    uint32 hash;
    uint32 nameidx_flags;   // Name index (lower 24 bits) and flags
    uint32 off;             // Offset
    uint32 csz;             // Compressed size
    uint32 sz;              // File size
} PkgDirEntDisk;
saDeclare(PkgDirEntDisk);

enum PKGDirEnt_Flags_Enum {
    PKG_Compressed = (1 << 24)   // Compressed with zlib
};
