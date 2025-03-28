// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQThreadPoolManager_ClassIf TQThreadPoolManager_ClassIf_tmpl = {
    ._size = sizeof(TQThreadPoolManager_ClassIf),
    ._parent = (ObjIface*)&TQManager_ClassIf_tmpl,
};

static TQThreadPoolManager_ClassIf _impl_TQThreadPoolManager_TQThreadPoolManager_ClassIf = {
    ._size = sizeof(TQThreadPoolManager_ClassIf),
    ._implements = (ObjIface*)&TQThreadPoolManager_ClassIf_tmpl,
    .updatePoolSize = (void (*)(void*))TQThreadPoolManager_updatePoolSize,
    .start = (bool (*)(void*, TaskQueue*))TQThreadPoolManager_start,
    .tick = (int64 (*)(void*))TQThreadPoolManager_tick,
    .stop = (bool (*)(void*))TQThreadPoolManager_stop,
};

static ObjIface* _ifimpl_TQThreadPoolManager[] = {
    (ObjIface*)&_impl_TQThreadPoolManager_TQThreadPoolManager_ClassIf,
    NULL
};

ObjClassInfo TQThreadPoolManager_clsinfo = {
    .instsize = sizeof(TQThreadPoolManager),
    .classif = (ObjIface*)&TQThreadPoolManager_ClassIf_tmpl,
    .parent = &TQManager_clsinfo,
    .destroy = (void(*)(void*))TQThreadPoolManager_destroy,
    ._abstract = true,
    .ifimpl = _ifimpl_TQThreadPoolManager,
};

// ==================== Auto-generated section ends ======================
