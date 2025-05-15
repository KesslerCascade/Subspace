#pragma once
// This header file is auto-generated!
// Do not make changes to this file or they will be overwritten.
// clang-format off
#include <cx/obj.h>
#include "pkgfs.h"

typedef struct VFSDir VFSDir;
typedef struct PkgFSFile PkgFSFile;
typedef struct PkgFSFile_WeakRef PkgFSFile_WeakRef;
saDeclarePtr(PkgFSFile);
saDeclarePtr(PkgFSFile_WeakRef);

typedef struct PkgFSFile_ClassIf {
    ObjIface* _implements;
    ObjIface* _parent;
    size_t _size;

    bool (*close)(_In_ void* self);
    bool (*read)(_In_ void* self, _Out_writes_bytes_to_(sz, *bytesread) void* buf, size_t sz, _Out_ _Deref_out_range_(0, sz) size_t* bytesread);
    bool (*write)(_In_ void* self, _In_reads_bytes_(sz) void* buf, size_t sz, _Out_opt_ _Deref_out_range_(0, sz) size_t* byteswritten);
    int64 (*tell)(_In_ void* self);
    int64 (*seek)(_In_ void* self, int64 off, FSSeekType seektype);
    bool (*flush)(_In_ void* self);
} PkgFSFile_ClassIf;
extern PkgFSFile_ClassIf PkgFSFile_ClassIf_tmpl;

typedef struct PkgFSFile {
    union {
        PkgFSFile_ClassIf* _;
        void* _is_PkgFSFile;
        void* _is_ObjInst;
    };
    ObjClassInfo* _clsinfo;
    atomic(uintptr) _ref;
    atomic(ptr) _weakref;

    PkgFS* prov;
    PkgDirEnt* ent;
    uint32 pos;
} PkgFSFile;
extern ObjClassInfo PkgFSFile_clsinfo;
#define PkgFSFile(inst) ((PkgFSFile*)(unused_noeval((inst) && &((inst)->_is_PkgFSFile)), (inst)))
#define PkgFSFileNone ((PkgFSFile*)NULL)

typedef struct PkgFSFile_WeakRef {
    union {
        ObjInst* _inst;
        void* _is_PkgFSFile_WeakRef;
        void* _is_ObjInst_WeakRef;
    };
    atomic(uintptr) _ref;
    RWLock _lock;
} PkgFSFile_WeakRef;
#define PkgFSFile_WeakRef(inst) ((PkgFSFile_WeakRef*)(unused_noeval((inst) && &((inst)->_is_PkgFSFile_WeakRef)), (inst)))

_objfactory_check PkgFSFile* PkgFSFile_create(PkgFS* prov, PkgDirEnt* ent);
// PkgFSFile* pkgfsfileCreate(PkgFS* prov, PkgDirEnt* ent);
#define pkgfsfileCreate(prov, ent) PkgFSFile_create(PkgFS(prov), PkgDirEnt(ent))

// bool pkgfsfileClose(PkgFSFile* self);
#define pkgfsfileClose(self) (self)->_->close(PkgFSFile(self))
// bool pkgfsfileRead(PkgFSFile* self, void* buf, size_t sz, size_t* bytesread);
#define pkgfsfileRead(self, buf, sz, bytesread) (self)->_->read(PkgFSFile(self), buf, sz, bytesread)
// bool pkgfsfileWrite(PkgFSFile* self, void* buf, size_t sz, size_t* byteswritten);
#define pkgfsfileWrite(self, buf, sz, byteswritten) (self)->_->write(PkgFSFile(self), buf, sz, byteswritten)
// int64 pkgfsfileTell(PkgFSFile* self);
#define pkgfsfileTell(self) (self)->_->tell(PkgFSFile(self))
// int64 pkgfsfileSeek(PkgFSFile* self, int64 off, FSSeekType seektype);
#define pkgfsfileSeek(self, off, seektype) (self)->_->seek(PkgFSFile(self), off, seektype)
// bool pkgfsfileFlush(PkgFSFile* self);
#define pkgfsfileFlush(self) (self)->_->flush(PkgFSFile(self))

