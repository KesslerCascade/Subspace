// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
VFSVFSFile_ClassIf VFSVFSFile_ClassIf_tmpl = {
    ._size = sizeof(VFSVFSFile_ClassIf),
};

static VFSFileProvider _impl_VFSVFSFile_VFSFileProvider = {
    ._size = sizeof(VFSFileProvider),
    ._implements = (ObjIface*)&VFSFileProvider_tmpl,
    .close = (bool (*)(void*))VFSVFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))VFSVFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))VFSVFSFile_write,
    .tell = (int64 (*)(void*))VFSVFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))VFSVFSFile_seek,
    .flush = (bool (*)(void*))VFSVFSFile_flush,
};

static VFSVFSFile_ClassIf _impl_VFSVFSFile_VFSVFSFile_ClassIf = {
    ._size = sizeof(VFSVFSFile_ClassIf),
    ._implements = (ObjIface*)&VFSVFSFile_ClassIf_tmpl,
    .close = (bool (*)(void*))VFSVFSFile_close,
    .read = (bool (*)(void*, void*, size_t, size_t*))VFSVFSFile_read,
    .write = (bool (*)(void*, void*, size_t, size_t*))VFSVFSFile_write,
    .tell = (int64 (*)(void*))VFSVFSFile_tell,
    .seek = (int64 (*)(void*, int64, FSSeekType))VFSVFSFile_seek,
    .flush = (bool (*)(void*))VFSVFSFile_flush,
};

static ObjIface* _ifimpl_VFSVFSFile[] = {
    (ObjIface*)&_impl_VFSVFSFile_VFSFileProvider,
    (ObjIface*)&_impl_VFSVFSFile_VFSVFSFile_ClassIf,
    NULL
};

ObjClassInfo VFSVFSFile_clsinfo = {
    .instsize = sizeof(VFSVFSFile),
    .classif = (ObjIface*)&VFSVFSFile_ClassIf_tmpl,
    .destroy = (void(*)(void*))VFSVFSFile_destroy,
    .ifimpl = _ifimpl_VFSVFSFile,
};

// ==================== Auto-generated section ends ======================
