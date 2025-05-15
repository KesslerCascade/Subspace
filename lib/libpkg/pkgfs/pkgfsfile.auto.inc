// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
PkgFSFile_ClassIf PkgFSFile_ClassIf_tmpl = {
    ._size = sizeof(PkgFSFile_ClassIf),
};

static VFSFileProvider _impl_PkgFSFile_VFSFileProvider = {
    ._size = sizeof(VFSFileProvider),
    ._implements = (ObjIface*)&VFSFileProvider_tmpl,
    .close = (bool (*)(void*))PkgFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))PkgFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))PkgFSFile_write,
    .tell = (int64 (*)(void*))PkgFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))PkgFSFile_seek,
    .flush = (bool (*)(void*))PkgFSFile_flush,
};

static PkgFSFile_ClassIf _impl_PkgFSFile_PkgFSFile_ClassIf = {
    ._size = sizeof(PkgFSFile_ClassIf),
    ._implements = (ObjIface*)&PkgFSFile_ClassIf_tmpl,
    .close = (bool (*)(void*))PkgFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))PkgFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))PkgFSFile_write,
    .tell = (int64 (*)(void*))PkgFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))PkgFSFile_seek,
    .flush = (bool (*)(void*))PkgFSFile_flush,
};

static ObjIface* _ifimpl_PkgFSFile[] = {
    (ObjIface*)&_impl_PkgFSFile_VFSFileProvider,
    (ObjIface*)&_impl_PkgFSFile_PkgFSFile_ClassIf,
    NULL
};

ObjClassInfo PkgFSFile_clsinfo = {
    .instsize = sizeof(PkgFSFile),
    .classif = (ObjIface*)&PkgFSFile_ClassIf_tmpl,
    .destroy = (void(*)(void*))PkgFSFile_destroy,
    .ifimpl = _ifimpl_PkgFSFile,
};

// ==================== Auto-generated section ends ======================
