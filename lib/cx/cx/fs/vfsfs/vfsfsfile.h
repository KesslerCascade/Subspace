#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "vfsfs.h"
#include <cx/fs/file.h>

typedef struct VFSFSFile VFSFSFile;
typedef struct VFSFSFile_WeakRef VFSFSFile_WeakRef;
saDeclarePtr(VFSFSFile);
saDeclarePtr(VFSFSFile_WeakRef);

typedef struct VFSFSFile_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*close)(_In_ void* self);
    bool (*read)(_In_ void* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread);
    bool (*write)(_In_ void* self, _In_reads_bytes_(sz) void* buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten);
    int64 (*tell)(_In_ void* self);
    int64 (*seek)(_In_ void* self, int64 off, FSSeekType seektype);
    bool (*flush)(_In_ void* self);
} VFSFSFile_ClassIf;
extern VFSFSFile_ClassIf VFSFSFile_ClassIf_tmpl;

typedef struct VFSFSFile {
    union {
        VFSFSFile_ClassIf* _;
        void* _is_VFSFSFile;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    FSFile* file;
} VFSFSFile;
extern ObjClassInfo VFSFSFile_clsinfo;
#define VFSFSFile(inst) ((VFSFSFile*)(unused_noeval((inst) && &((inst)->_is_VFSFSFile)), (inst)))
#define VFSFSFileNone ((VFSFSFile*)NULL)

typedef struct VFSFSFile_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_VFSFSFile_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} VFSFSFile_WeakRef;
#define VFSFSFile_WeakRef(inst) ((VFSFSFile_WeakRef*)(unused_noeval((inst) && &((inst)->_is_VFSFSFile_WeakRef)), (inst)))

_objfactory_guaranteed VFSFSFile* VFSFSFile_create(FSFile* f);
// VFSFSFile* vfsfsfileCreate(FSFile* f);
#define vfsfsfileCreate(f) VFSFSFile_create(f)

// bool vfsfsfileClose(VFSFSFile* self);
#define vfsfsfileClose(self) (self)->_->close(VFSFSFile(self))
// bool vfsfsfileRead(VFSFSFile* self, void* buf, size_t sz, size_t* bytesread);
#define vfsfsfileRead(self, buf, sz, bytesread) (self)->_->read(VFSFSFile(self), buf, sz, bytesread)
// bool vfsfsfileWrite(VFSFSFile* self, void* buf, size_t sz, size_t* byteswritten);
#define vfsfsfileWrite(self, buf, sz, byteswritten) (self)->_->write(VFSFSFile(self), buf, sz, byteswritten)
// int64 vfsfsfileTell(VFSFSFile* self);
#define vfsfsfileTell(self) (self)->_->tell(VFSFSFile(self))
// int64 vfsfsfileSeek(VFSFSFile* self, int64 off, FSSeekType seektype);
#define vfsfsfileSeek(self, off, seektype) (self)->_->seek(VFSFSFile(self), off, seektype)
// bool vfsfsfileFlush(VFSFSFile* self);
#define vfsfsfileFlush(self) (self)->_->flush(VFSFSFile(self))

