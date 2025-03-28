// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQInWorkerManager_ClassIf TQInWorkerManager_ClassIf_tmpl = {
    ._size = sizeof(TQInWorkerManager_ClassIf),
    ._parent = (ObjIface*)&TQThreadPoolManager_ClassIf_tmpl,
};

static TQInWorkerManager_ClassIf _impl_TQInWorkerManager_TQInWorkerManager_ClassIf = {
    ._size = sizeof(TQInWorkerManager_ClassIf),
    ._implements = (ObjIface*)&TQInWorkerManager_ClassIf_tmpl,
    .notify = (void (*)(void*, bool))TQInWorkerManager_notify,
    .tick = (int64 (*)(void*))TQInWorkerManager_tick,
    .pretask = (void (*)(void*))TQInWorkerManager_pretask,
    .stop = (bool (*)(void*))TQInWorkerManager_stop,
};

static ObjIface* _ifimpl_TQInWorkerManager[] = {
    (ObjIface*)&_impl_TQInWorkerManager_TQInWorkerManager_ClassIf,
    NULL
};

ObjClassInfo TQInWorkerManager_clsinfo = {
    .instsize = sizeof(TQInWorkerManager),
    .classif = (ObjIface*)&TQInWorkerManager_ClassIf_tmpl,
    .parent = &TQThreadPoolManager_clsinfo,
    .init = (bool(*)(void*))TQInWorkerManager_init,
    .destroy = (void(*)(void*))TQInWorkerManager_destroy,
    .ifimpl = _ifimpl_TQInWorkerManager,
};

// ==================== Auto-generated section ends ======================
