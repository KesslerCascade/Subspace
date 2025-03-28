#pragma once

// CX basic low-level file i/o

// This acts as a bridge to the operating system. It does synchronous I/O with
// only the OS-provided buffering. Unlike the C standard library, I/O is not
// line-oriented in any way.

#include <cx/cx.h>

CX_C_BEGIN

typedef struct FSFile FSFile;

enum FSOpenFlags {
    FS_Read     = 1,
    FS_Write    = 2,
    FS_Create   = 4,
    FS_Truncate = 8,
    FS_Lock     = 16,
    FS_Overwrite = (FS_Write | FS_Create | FS_Truncate),
};

typedef enum FSSeekTypeEnum {
    FS_Set      = 0x00010000,
    FS_Cur      = 0x00020000,
    FS_End      = 0x00030000,
} FSSeekType;

_Ret_opt_valid_ FSFile *fsOpen(_In_opt_ strref path, flags_t flags);
bool fsClose(_Pre_valid_ _Post_invalid_ FSFile *file);

bool fsRead(_Inout_ FSFile *file, _Out_writes_bytes_to_(sz, *bytesread) void *buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t *bytesread);
bool fsWrite(_Inout_ FSFile *file, _In_reads_bytes_(sz) void *buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t *byteswritten);
int64 fsTell(_Inout_ FSFile *file);
int64 fsSeek(_Inout_ FSFile *file, int64 off, FSSeekType seektype);

bool fsFlush(_Inout_ FSFile *file);

CX_C_END
