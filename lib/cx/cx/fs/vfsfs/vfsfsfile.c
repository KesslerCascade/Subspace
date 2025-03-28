// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "vfsfsfile.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed VFSFSFile* VFSFSFile_create(FSFile* f)
{
    VFSFSFile *ret;
    ret = objInstCreate(VFSFSFile);
    ret->file = f;
    objInstInit(ret);
    return ret;
}

bool VFSFSFile_close(_In_ VFSFSFile* self)
{
    bool ret = false;
    if (self->file)
        ret = fsClose(self->file);
    self->file = NULL;
    return ret;
}

bool VFSFSFile_read(_In_ VFSFSFile* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread)
{
    if (!self->file) {
        *bytesread = 0;
        return false;
    }
    return fsRead(self->file, buf, sz, bytesread);
}

bool VFSFSFile_write(_In_ VFSFSFile* self, _In_reads_bytes_(sz) void* buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten)
{
    if (!self->file) {
        if (byteswritten)
            *byteswritten = 0;
        return false;
    }
    return fsWrite(self->file, buf, sz, byteswritten);
}

int64 VFSFSFile_tell(_In_ VFSFSFile* self)
{
    if (!self->file)
        return -1;
    return fsTell(self->file);
}

int64 VFSFSFile_seek(_In_ VFSFSFile* self, int64 off, FSSeekType seektype)
{
    if (!self->file)
        return -1;
    return fsSeek(self->file, off, seektype);
}

bool VFSFSFile_flush(_In_ VFSFSFile* self)
{
    if (!self->file)
        return false;
    return fsFlush(self->file);
}

void VFSFSFile_destroy(_In_ VFSFSFile* self)
{
    VFSFSFile_close(self);
}

// Autogen begins -----
#include "vfsfsfile.auto.inc"
// Autogen ends -------
