// ==================== Auto-generated section begins ====================
// clang-format off
// Do not modify the contents of this section; any changes will be lost!
TQThreadPoolRunner_ClassIf TQThreadPoolRunner_ClassIf_tmpl = {
    ._size = sizeof(TQThreadPoolRunner_ClassIf),
    ._parent = (ObjIface*)&TQRunner_ClassIf_tmpl,
};

static TQThreadPoolRunner_ClassIf _impl_TQThreadPoolRunner_TQThreadPoolRunner_ClassIf = {
    ._size = sizeof(TQThreadPoolRunner_ClassIf),
    ._implements = (ObjIface*)&TQThreadPoolRunner_ClassIf_tmpl,
    .addWorker = (bool (*)(void*))TQThreadPoolRunner_addWorker,
    .removeWorker = (bool (*)(void*))TQThreadPoolRunner_removeWorker,
    .createWorker = (TQThreadWorker* (*)(void*, int32))TQThreadPoolRunner_createWorker,
    .start = (bool (*)(void*, TaskQueue*))TQThreadPoolRunner_start,
    .stop = (bool (*)(void*))TQThreadPoolRunner_stop,
    .tick = (int64 (*)(void*))TQThreadPoolRunner_tick,
};

static ObjIface* _ifimpl_TQThreadPoolRunner[] = {
    (ObjIface*)&_impl_TQThreadPoolRunner_TQThreadPoolRunner_ClassIf,
    NULL
};

ObjClassInfo TQThreadPoolRunner_clsinfo = {
    .instsize = sizeof(TQThreadPoolRunner),
    .classif = (ObjIface*)&TQThreadPoolRunner_ClassIf_tmpl,
    .parent = &TQRunner_clsinfo,
    .init = (bool(*)(void*))TQThreadPoolRunner_init,
    .destroy = (void(*)(void*))TQThreadPoolRunner_destroy,
    .ifimpl = _ifimpl_TQThreadPoolRunner,
};

// ==================== Auto-generated section ends ======================
