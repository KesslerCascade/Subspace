// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
VFSFSFile_ClassIf VFSFSFile_ClassIf_tmpl = {
    ._size = sizeof(VFSFSFile_ClassIf),
};

static VFSFileProvider _impl_VFSFSFile_VFSFileProvider = {
    ._size = sizeof(VFSFileProvider),
    ._implements = (ObjIface*)&VFSFileProvider_tmpl,
    .close = (bool (*)(void*))VFSFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))VFSFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))VFSFSFile_write,
    .tell = (int64 (*)(void*))VFSFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))VFSFSFile_seek,
    .flush = (bool (*)(void*))VFSFSFile_flush,
};

static VFSFSFile_ClassIf _impl_VFSFSFile_VFSFSFile_ClassIf = {
    ._size = sizeof(VFSFSFile_ClassIf),
    ._implements = (ObjIface*)&VFSFSFile_ClassIf_tmpl,
    .close = (bool (*)(void*))VFSFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))VFSFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))VFSFSFile_write,
    .tell = (int64 (*)(void*))VFSFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))VFSFSFile_seek,
    .flush = (bool (*)(void*))VFSFSFile_flush,
};

static ObjIface* _ifimpl_VFSFSFile[] = {
    (ObjIface*)&_impl_VFSFSFile_VFSFileProvider,
    (ObjIface*)&_impl_VFSFSFile_VFSFSFile_ClassIf,
    NULL
};

ObjClassInfo VFSFSFile_clsinfo = {
    .instsize = sizeof(VFSFSFile),
    .classif = (ObjIface*)&VFSFSFile_ClassIf_tmpl,
    .destroy = (void(*)(void*))VFSFSFile_destroy,
    .ifimpl = _ifimpl_VFSFSFile,
};

// ==================== Auto-generated section ends ======================
