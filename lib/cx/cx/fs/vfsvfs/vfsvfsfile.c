// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "vfsvfsfile.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_guaranteed VFSVFSFile* VFSVFSFile_create(VFSFile* f)
{
    VFSVFSFile *ret;
    ret = objInstCreate(VFSVFSFile);
    ret->file = f;
    objInstInit(ret);
    return ret;
}

bool VFSVFSFile_close(_In_ VFSVFSFile* self)
{
    bool ret = false;
    if (self->file)
        ret = vfsClose(self->file);
    self->file = NULL;
    return ret;
}

bool VFSVFSFile_read(_In_ VFSVFSFile* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread)
{
    if (!self->file) {
        *bytesread = 0;
        return false;
    }
    return vfsRead(self->file, buf, sz, bytesread);
}

bool VFSVFSFile_write(_In_ VFSVFSFile* self, _In_reads_bytes_(sz) void* buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten)
{
    if (!self->file) {
        if (byteswritten)
            *byteswritten = 0;
        return false;
    }
    return vfsWrite(self->file, buf, sz, byteswritten);
}

int64 VFSVFSFile_tell(_In_ VFSVFSFile* self)
{
    if (!self->file)
        return -1;
    return vfsTell(self->file);
}

int64 VFSVFSFile_seek(_In_ VFSVFSFile* self, int64 off, FSSeekType seektype)
{
    if (!self->file)
        return -1;
    return vfsSeek(self->file, off, seektype);
}

bool VFSVFSFile_flush(_In_ VFSVFSFile* self)
{
    if (!self->file)
        return false;
    return vfsFlush(self->file);
}

void VFSVFSFile_destroy(_In_ VFSVFSFile* self)
{
    VFSVFSFile_close(self);
}

// Autogen begins -----
#include "vfsvfsfile.auto.inc"
// Autogen ends -------
