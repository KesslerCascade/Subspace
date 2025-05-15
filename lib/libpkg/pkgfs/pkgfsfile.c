// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
#include <cx/obj.h>
#include <cx/debug/assert.h>
#include <cx/obj/objstdif.h>
#include <cx/container.h>
#include <cx/string.h>
#include "pkgfsfile.h"
// clang-format on
// ==================== Auto-generated section ends ======================

_objfactory_check PkgFSFile* PkgFSFile_create(PkgFS* prov, PkgDirEnt* ent)
{
    PkgFSFile* self;
    self = objInstCreate(PkgFSFile);

    self->prov = objAcquire(prov);
    self->ent  = objAcquire(ent);

    if (!objInstInit(self)) {
        objRelease(&self);
        return NULL;
    }

    return self;
}

bool PkgFSFile_close(_In_ PkgFSFile* self)
{
    objRelease(&self->prov);
    objRelease(&self->ent);
    self->pos = 0;
    return true;
}

static bool PkgFSFile_readCompressed(_In_ PkgFSFile* self,
                                     _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz,
                                     _Out_ _Deref_out_range_(0, sz) size_t* bytesread)
{
    // TODO: Handle compressed files. This should be completed for full pkg format compatibility,
    // but none of the supported versions of FTL seem to actually have any compressed files in the
    // package, so it's not needed right away.
    return false;
}

bool PkgFSFile_read(_In_ PkgFSFile* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf,
                    size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread)
{
    uint32 canread = (uint32)min(sz, self->ent->size - self->pos);
    size_t didread = 0;
    bool ret       = false;

    // special case for EOF
    if (self->pos == self->ent->size) {
        *bytesread = 0;
        return true;
    }

    if (!self->ent->compressed) {
        // just read from the backing file directly
        withMutex (&self->prov->pkg->filelock) {
            if (vfsSeek(self->prov->pkg->pkgfile, self->ent->off + self->pos, FS_Set) &&
                vfsRead(self->prov->pkg->pkgfile, buf, canread, &didread))
                ret = true;
        }
    } else {
        ret = PkgFSFile_readCompressed(self, buf, sz, &didread);
    }

    self->pos += (uint32)didread;
    *bytesread = (uint32)didread;
    return true;
}

bool PkgFSFile_write(_In_ PkgFSFile* self, _In_reads_bytes_(sz) void* buf, size_t sz,
                     _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten)
{
    if (byteswritten)
        *byteswritten = 0;

    return false;
}

int64 PkgFSFile_tell(_In_ PkgFSFile* self)
{
    return self->pos;
}

int64 PkgFSFile_seek(_In_ PkgFSFile* self, int64 off, FSSeekType seektype)
{
    switch (seektype) {
    case FS_Set:
        self->pos = (uint32)clamp(off, 0, self->ent->size);
        break;
    case FS_Cur:
        self->pos = (uint32)clamp(self->pos + off, 0, self->ent->size);
        break;
    case FS_End:
        self->pos = (uint32)clamp(self->ent->size - off, 0, self->ent->size);
        break;
    }
    return self->pos;
}

bool PkgFSFile_flush(_In_ PkgFSFile* self)
{
    return false;
}

void PkgFSFile_destroy(_In_ PkgFSFile* self)
{
    // Autogen begins -----
    objRelease(&self->prov);
    objRelease(&self->ent);
    // Autogen ends -------
}

// Autogen begins -----
#include "pkgfsfile.auto.inc"
// Autogen ends -------
